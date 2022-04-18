#include <iostream>
#include <sstream>
#include <deque>
#include <cstdint>
const size_t N = 26 + 26 + 10;
//const size_t N = 3;
const size_t EDGES = N * N * N * N;
const uint32_t MAX_NODE = ((N - 1) | ((N - 1) << 8)) | (((N - 1) | ((N - 1) << 8)) << 16);
const uint32_t INVALID_ID = std::numeric_limits<uint32_t>::max();
bool is_necklace(uint32_t x) {
    for (int i = 1; i < 4; i++)
        if (((x << (8 * i)) | (x >> (8 * (4 - i)))) > x) return false;
    return true;
}
uint32_t next_necklace(uint32_t x) {
    int j = 3; 
    if (x == 0)return MAX_NODE;
    int bytes = 4;
    while ((x & 255) == 0) { x = x >> 8; bytes--; }
    x--;
    while (bytes < 4) {
        int t = 4 - bytes;
        if (t > bytes)t = bytes;
        x = (x << (8 * t)) | (x >> (8*(bytes - t)));
        bytes += t;
    }
    if (is_necklace(x))return x;
    return next_necklace(x);
}

char itoc(int id) {
    if (id < 0 || id >= N)exit(1);
    if (id < 26)return 'A' + id;
    if (id < 52)return 'a' + id - 26;
    return '0' + id - 52;
}
uint32_t history = INVALID_ID;
bool output_char(uint8_t ch, uint32_t lookup) {
    if (lookup == INVALID_ID) {
        printf("%c", itoc(ch));
        return false;
    }
    else {
        history = (history << 8) | static_cast<uint32_t>(ch);
        return(history == lookup);
    }
}
int reduce_and_print(uint32_t x, size_t len, uint32_t lookup = INVALID_ID) {
    if (
        (x % (1 << 8) == (x >> 8) % (1 << 8)) &&
        (x % (1 << 8) == (x >> 16) % (1 << 8)) &&
        (x % (1 << 8) == (x >> 24) % (1 << 8))
        )
    {
        return output_char(x & 255, lookup) ? -1 : 1;
    }
    if (x / (1 << 16) == x % (1 << 16)) {
        if (len > 2)len = 2;
        for (int j = len - 1; j >= 0; j--) {
            if (output_char(x >> (8 * j), lookup))return j - len;
        }
        return len;
    }
    if (len > 4)len = 4;
    for (int j = len - 1; j >= 0; j--) {
        if (output_char(x >> (8 * j), lookup))return j - len;
    }
    return len;
}

uint64_t generate(uint64_t len, uint32_t lookup, bool skip_null) {
    history = INVALID_ID;
    uint32_t start = MAX_NODE;
    start = start | (start << 8);
    start = start | (start << 16);
    len -= reduce_and_print(start, len,lookup);
    start = next_necklace(start);
    uint64_t current_cnt = 0;
    uint64_t find_id = std::numeric_limits<uint64_t>::max();
    while (len) {
        if (skip_null && start == 0)start = MAX_NODE;
        auto t = reduce_and_print(start, len, lookup);
        if (t > 0) {
            len -= t;
            current_cnt += t;
        }
        else {
            find_id = current_cnt + (-t) - 3;
            break;
        }
        if (lookup != INVALID_ID && start == MAX_NODE)break;
        start = next_necklace(start);
    }
    return find_id;
}

int ctoi(char c) {
    if (c >= 'A' && c <= 'Z')return c - 'A';
    if (c >= 'a' && c <= 'z')return c - 'a' + 26;
    if (c >= '0' && c <= '9')return c - '0' + 52;
    exit(1);
    return -1;
}
void lookup(size_t len, uint32_t data, uint32_t data2)
{
    
    uint32_t edge = 0;
    for (int i = 0; i < 4; i++) {
        edge = (edge << 8) | ctoi(data & 255);
        data = data >> 8;
    }
    uint64_t result = generate(len, edge, false);
    if (result == std::numeric_limits<uint64_t>::max()) {
        fprintf(stderr, "Not found.");
        return;
    }
    uint64_t mod = EDGES;
    if (data2 != INVALID_ID) {
        mod = EDGES * (EDGES - 1);
        uint32_t edge2 = 0;
        for (int i = 0; i < 4; i++) {
            edge2 = (edge2 << 8) | ctoi(data2 & 255);
            data2 = data2 >> 8;
        }
        uint64_t m2 = generate(len, edge2, true);
        if (m2 == std::numeric_limits<uint64_t>::max()) {
            fprintf(stderr, "Not found.");
            return;
        }
        result = EDGES * m2 + (EDGES - 1) * (EDGES - result);
        result = result % mod;
    }
    if (result <= len - 4) {
        printf("%I64d", result);
        unsigned long long tmp = result + mod;
        while (tmp <= len - 4) {
            printf(", %I64d", tmp);
            tmp += mod;
        }
    }
    else {
        fprintf(stderr, "Not found.");
        return;
    }
}

int main(int argc,char** argv)
{
    if (strcmp(argv[1], "generate") == 0 || strcmp(argv[1], "generate2") == 0) {
        uint64_t len = strtoull(argv[2], nullptr, 10);
        if (len == 0)return 0;
        if (strcmp(argv[1], "generate") == 0)
            generate(len, INVALID_ID, false);
        else
            generate(len, INVALID_ID, true);
    }
    if (strcmp(argv[1], "lookup") == 0) {
        uint64_t len = strtoull(argv[2], nullptr, 10);
        uint32_t data = strtoull(argv[3], nullptr, 16);
        uint32_t data2 = INVALID_ID;
        if (argc >= 5)data2 = strtoull(argv[4], nullptr, 16);
        if (len == 0 || data == 0 || data2 == 0)return 0;
        
        lookup(len, data, data2);
    }
    return 0;
}

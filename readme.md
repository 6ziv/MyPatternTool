## MyPatternTool: ultra-long buffer overflow pattern generator 

#### Introduction

On one of my courses, a small tool named "PatternTool" was used to generate unique pattern to find the offset of an overwritten address.

The pattern it generates is almost the same as what we can find on https://wiremask.eu/tools/buffer-overflow-pattern-generator/, with a recurring period of about 2028 bytes.

So, if you need a payload longer than that, you may get multiple offsets, and need to manually construct another pattern to distinguish them.

That makes my wonder if I can do better.

From some really early discussion with my high-school classmate, I've learned that, for any alphabet with size M and any positive number N, there exists a (M^N)-length string whose N-length substrings (in a circular manner) are exactly all unique N-length strings on the given alphabet. Such string is called a De-Bruijn sequence.

He gave an elegant proof, which I will mention later, using Euler's theorem in Graph Theory. However, that doesn't mean I can generate the string efficiently.

Yes, by using a common way to "generate" a Euler circuit (by always choosing an unvisited path to walk), I can do it in O(M^N) time, which is theoretically the best (after all, we need time to output). But the memory usage will be at least O(M^(N-1)).

Luckily, when going through the references of homework form another course, I found some old papers concerning this problem, and thus got a much-lower memory usage.



#### Algorithm FM

Well, this algorithm can be found in [this paper](https://doi.org/10.1080/0025570X.1982.11976970 ). For those who cannot access the paper, I will describe it briefly.

We represent each character in the alphabet with in integer, so a string is represented by a sequence of integers. And we compare sequences with dictionary comparison.

We can "rotate" a sequence by splitting it into two parts and swap them. 

A necklace is a N-length sequence which is not smaller than any of its "rotations".

For example: 2,1,0,2 is not a necklace (because 2,2,1,0 is larger than it), but 2,1,2,1 is.

The paper mentioned above shows a way to list all necklaces in descending order. Starting from the largest one (for example, 2222). Each time, we decrease the lowest non-zero digit, and fill the lower digits by repeating digits not-lower than this.

For example, lowest non-zero digit in sequence 2,2,1,0,0,0,0,0 is 1 on the third highest digit. So we decrease it by one, and fill the five lower digits by repeating "2,2,0". So the resulting sequence is "2,2,0,2,2,0,2,2".

Check whether it is a necklace. If not, restart from this sequence.

Now we print all necklaces in descending order. If a necklace is a repeat of a shorter sequence, we print the shortest recurring section instead of the necklace itself.

By doing so, we printed a De-Bruijn sequence, generating a pattern whose N-length substring is unique in the first (M^N+N-1) characters, and repeats every M^N characters.



##### And more?

With alphabet={uppercase letters, lowercase letters, digits} and N=4, we know that the period is 14776336 bytes (about 14 Megabytes).

And what if we need to use it on a longer sequence? The naive way is that we manually replace all occurrences of the seen 4-byte into different ones, and redo the experiment.

But now, to be a little "tricky": we simply remove the last character from every recurring period, and thus get a new "pattern".

Let's go back to the "Euler circuit" proof:

We construct a graph with each vertex representing a (N-1)-length string, and each directed edge represents a N-length string, whose first N-1 characters is its source, and last N-1 characters is its destination. So each node has in-degree M and out-degree M (by prefixing or suffixing any of the M characters in alphabet). Obviously, the De-Bruijn sequence is a Euler circuit in this graph.

Now we simply remove a cycle (for example, 0->0) from the circuit. So the rest still forms a circuit, containing all edges expect the dropped one, which means we get a recurring string whose period is exactly 1 smaller than the De-Bruijn sequence, and whose N-length substrings in a recurring sections are distinct.

So combining the two patterns, we get two modular equations, whose moduli are M^N and M^N-1, so we can merge them into one equation with module (M^N)x(M^N-1). For M=62 and N=4, this is 218,340,090,808,560 bytes, far enough to determine the offset of an overwritten address.



#### Build and Use

Simply compile the cpp file to build.

Supported usages are listed as follows.

`MyPatternTool generate [len]` print first [len] bytes of the recurring pattern.

`MyPatternTool generate2 [len]` print first [len] bytes of the recurring pattern without "AAAA" (by skipping the last 'A').

`MyPatternTool lookup [len] [hex]` search for [hex] (little endian) in the first [len] bytes of the pattern.

`MyPatternTool lookup [len] [hex1] [hex2]` search for a position matching [hex1] in the full pattern and [hex2] in the pattern without "AAAA" in the first [len] bytes.



#### License

WTFPL
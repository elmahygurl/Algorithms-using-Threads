# Algorithms using Threads

## Project Overview

Implementing *Merge Sort* and *Matrix Multiplication* using POSIX Thread.


Two Matrix Multiplication Approaches:

1. The computation of each element of the output matrix happens in a thread.
2. The computation of each row of the output matrix happens in a thread.

The times between both approaches are calculated and printed for comparisons. 

## Test Cases 

You can find test case files for both matrix and sort algorithms to test with.

### Matrix Multiplication

Example test cases: 
- input1-matrix.txt
- input2-matrix.txt
- input3-matrix.txt
- input4-matrix.txt
- input5-matrix.txt
- input6-matrix.txt
- matrices.txt

### Merge Sort

Example test cases: 
- sorttt.txt
- input-sort.txt
- input-sort2.txt

For matrix test cases, the format is as follows:
[number of rows of 1st matrix] [number of columns of 1st matrix]

1st matrix entries

[number of rows of 2nd matrix] [number of columns of 2nd matrix]

2nd matrix entries

For array test cases, the format is:
[number of elements]
Array elements that are space-separated.


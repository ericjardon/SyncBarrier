# SyncBarrier

A research project on the famous synchronization mechanism known as Synchronization Barriers, made as a final project for the TC2025 Advanced Programming course at ITESM.

The work is inspired by [this article](https://medium.com/@jaydesai36/barrier-synchronization-in-threads-3c56f947047).

Synchronization barriers are used to enable multiple threads or processes to wait for each other at a certain point in execution.
This is very useful for phased operations, like computing the power of a square matrix.

For learning purposes, this implementation makes use of other built-in synchronization methods in C:
- Counting semaphores
- Binary semaphores
- Shared memory blocks
- The `pthread` library

**To run the demostration program, run `make` in your terminal**.
The input matrix can be typed in space-separated columns and line-separated rows, like so:   

    2 1 4 -2
    3 16 1 0
    1 1 -6 -1


### June 2021 updates
The Matrix multiplication demo program has been completed.
Given a square Matrix of size N and an power K, the program computes the K-th power of the input matrix using N processes and K-1 synchronization barriers. 

Testcases will be uploaded soon. 

The Sync Barrier implementation has also been upgraded to generate random semaphore names so the client application does not have to set them up.
This allows for chained operations that require multiple barriers in a single go.

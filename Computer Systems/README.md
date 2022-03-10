## Relevent Files for Allocator:
 - [mm.c](https://github.com/ggstrader/Code-Samples/blob/main/Computer%20Systems/Allocator%20(malloc%2Bfree)/mm.c)

*Demonstrates:*
Understanding of space and time constraints of memory allocation.
Features include bit packing, ghost footer blocks, the option for first/best-fit, and a doubly-linked free list

## Relevent Files for Performance Tuning:
 - [kernels.c](https://github.com/ggstrader/Code-Samples/blob/main/Computer%20Systems/Performance%20Tuning/kernels.c)

*Demonstrates:* 
Understanding of common performance optimization techniques like:
    loop unrolling, vectorization, temporal/spatial locality, etc.

## Relevent Files for Process Management:
 - [run_tournament.c](https://github.com/ggstrader/Code-Samples/blob/main/Computer%20Systems/Process%20Management/run_tournament.c)

*Demonstrates:* 
Understanding of process managment and signals in Linux.
At a high level, different processes must communicate with a parent process to determine whether they are "hot or cold" to the target on a 100*100 grid. 
This project is a process manager that creates the child processes and fascilitates communication between them, and is resilient against failures in the child processes.

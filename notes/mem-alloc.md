# Memory Allocation Strategies

## Sources

- [gingerBill Memory Allocation Strategies](https://www.gingerbill.org/series/memory-allocation-strategies/).
- [molecular musings blog](https://blog.molecular-matters.com/).

## Introduction

Many languages try to automatically handle memory by using different strategies, like:

- Garbage collection (GC).
- Automatic reference counting (ARC).
- Resource acquisition is initialization (RAII).
- Ownership semantics.
This is done, so the programmer does not have to think or work with memory. But abstracting memory comes with its own cost.

When we do start "manually" managing memory, we normally think about memory in terms of the running process with a dualistic model. This model divides the
memory in two parts. The stack, containing function frames and the heap, containing memory which is allocated during runtime (as opposed to allocation during compile time).
This dualistic model is in itself already an abstraction of process memory.

Modern operating systems virtualize memory on per-process basis. This means that each address used in a process is specific only to that process and its virtual memory.
Memory is thus in a monistic model where everything is virtual memory. Some of that virtual address space is reserved for procedure stack frames, some is required by the OS,
and the rest is used for virtual memory. The biggest take away from this is that memory is virtually-mapped and linear, and that linear memory space can be split into sections.

## Allocating Memory

There are three main aspects to think about when it comes to allocation:

- The size of allocation.
- The lifetime of that memory.
- The usage of that memory.

Usage and lifetime often correlate, so we need to only create four main memory allocation scenarios. This four scenarios depend on lifetimes and sizes.

1. Lifetime is **known**, size is **known**: This is the most common case. Even if we do not known the exact size, an upper bound is known.
2. Lifetime is **known**, size is **unknown**: In this case we do know how long the memory will be in use, but exact or bounded size in not known. Most common cases
are loading a file, populating a hash table, storing/working with user input, etc. In this cases reallocation is often used, since we might need more memory.
3. Lifetime is **unknown**, size is **known**: In this case "ownership" of that memory across multiple systems is ill-defined. A common solution for this domain of problems
is reference counting or ownership semantics.
4. Lifetime is **unknown**, size is **unknown**: This case is rare and normally a solution is to use a garbage collector.

There are also additional memory concerns, such as:

- Memory fragmentation.
- Memory safety.
- Memory leakage.

## Memory Lifetimes

Memory lifetimes can also be organized by their allocation generation.

- **Permanent Allocation**: Memory that is never freed until the end of the program. This memory is persistent during program lifetime.
- **Transient Allocation**: Memory that has a cycle-based lifetime. The memory only persists for the "cycle" and is freed at the end of this cycle.
- **Scratch/Temporary Allocation**: Short lived, quick memory.


## Memory allocators

- [Linear Allocators](./lin-alloc.md).
- [Stack Allocators](./stack-alloc.md).

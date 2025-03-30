# Buddy Allocators

## Sources

- [gingerBill Memory Allocation Strategies](https://www.gingerbill.org/article/2021/12/02/memory-allocation-strategies-006/).
- [Memory fragmentation](https://en.wikipedia.org/wiki/Fragmentation_(computing)).
- [Buddy memory allocation](https://en.wikipedia.org/wiki/Buddy_memory_allocation).

## Buddy Memory Allocation

One of the big problems with free lists is that they are very susceptible to internal memory fragmentation due to
allocations being of any size. If we still require the properties of free lists but want to reduce internal memory
fragmentation, the Buddy algorithm works in a similar principle.

## The Algorithm


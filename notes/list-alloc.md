# Free List Allocators

## Sources

- [gingerBill Memory Allocation Strategies](https://www.gingerbill.org/article/2019/02/15/memory-allocation-strategies-005/).

## Free List Based Allocation

Pool allocator splits the backing buffer into equal chunks and keeps track of which of the chunks are free. Pools are fast allocators that allow
for *out of order free* in constant time, whilst keeping very little fragmentation. The main restriction is that every memory allocation must be
of the same size.

A free list is a general purpose allocator, that does not impose any restrictions. It allows allocations and deallocations to be our of order and
of any size. Due to its nature, the allocator's performance is not as good as linear allocators.

## Link List Approach

We will use a linked list to store the address of free contiguous block in the memory along with its size. When the user request memory, it searches
in the linked list for a block where the data can fit. It then removes the element from the list and places an allocation header (required for free)
just before the data.

For freeing memory, we recover the allocation header (stored before the allocation) to know the size of the block we want to free. Once that block
has been freed, it is inserted into the linked list, and then we try to *coalescence* contiguous blocks of memory together to create larger blocks.

## Linked List Free List Implementation



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

The Buddy algorithms assumes that the backing memory block is a power-of-two in bytes. When an allocation is requested,
the allocator looks for a block whose size is at least the size of the requested allocation. If the requested allocation
size is less than half of the block, it is split into two (left and right), and the two resulting blocks are called
*buddies*. If this requested allocation size is still less than half of the left buddy, the buddy block is recursively
split until the resulting buddy is as small as possible to fit the requested allocation size.

When a block is released, we can try to perform coalescence on buddies (contiguous neighbouring blocks). There are
particular conditions that are needed. Coalescence cannot be performed if a block has no free buddy, the block is still
in use, or the buddy block is partially used.

## The Implementation

Each block has a header which stores information about it *in-line*. It stores its size and whether it is free.

```C
typedef struct Buddy_Block Buddy_Block;
struct Buddy_Block {
    size_t size; 
    bool   is_free;
};
```

We do not need to store a pointer to the next buddy block as we can calculate it directly from the stored size.

```C
Buddy_Block *buddy_block_next(Buddy_Block *block) {
    return (Buddy_Block *)((char *)block + block->size);
}
```

**Note**: many implementations of a buddy allocator use a doubly linked list here and store explicit pointers, which
allows for easier coalescence of neighbouring buddies and forward and backwards traversal. However this increases the
size of the allocation header for the memory block.

### Recursive Split

To get the best fitting block a recursive splitting algorithm is required. We need to continually split a block until it
is the optimal size for the allocation of the requested size.

### Finding the Best Block

Searching for a free block that matches the requested allocation size can be achieved by traversing an implicit linked
list bounded by `head` and `tail` pointers. If a block for the requested allocation size cannot be found, but there is
a larger free block, the above splitting algorithm is used. If there is no free block, it returns NULL to represent OOM.
Note that OOM in this case does not mean that all memory is used, it means there is not enough memory to perform an 
allocation of the requested size, but smaller allocations might still be possible.

This algorithm can suffer from undue internal fragmentation. You can coalesce on neighbouring free buddies as you
iterate.

### Initialization

The allocator stores the `head` block, a sentinel pointer `tail` which represents the upper memory boundary of the
backing memory data `((char *)head + size)`, and the alignment for each allocation.

**Note**: This implementation of a buddy allocator does require that all allocations must have the same alignment in
order to simplify the code a lot. Buddy allocators are usually a single strategy as part of a more complicated allocator
and this the assumption of alignment is less of an issue in practice.

### Allocation

We first increase requested allocation size to fit the header and align forward before we find a best fitting block.
If one is found, we then need to offset from the header to the usable data. If a block cannot be found, we can keep
coalescing any free blocks until we cannot coalesce any more and then try to look for a usable block again. If no
block is found, we return `NULL` to signify that we are OOM with this allocator.

The time complexity of this allocation algorithms is **O(N)** on average but a space complexity of **O(log(N))**.

**Note**: buddy allocators are still susceptible to internal fragmentation, but in practice, it is less than a normal
free list allocator because of the power-of-two restriction.

### Freeing Memory

All we need to do is mark the header as being free. The time-complexity of freeing memory is **O(1)**. If you wanted to,
coalescence could be performed straight after this free to aid in minimizing internal fragmentation.

## Conclusion

The buddy allocator is a powerful allocator and a conceptually simple algorithm but implementing it efficiently is a lot
harder than all the previous allocators.

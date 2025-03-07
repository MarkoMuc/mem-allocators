# Stack Allocators

## Sources

- [gingerBill Memory Allocation Strategies](https://www.gingerbill.org/article/2019/02/15/memory-allocation-strategies-003/).

## Stack-Like (LIFO) Allocation

This part covers fixed-sized stack-like allocator, also referred to as a *stack allocator*.

A stack-like allocator means that the allocator acts like a data structure following the last-in, first-out (LIFO)
principle. This has nothing to do with the stack or stack frame.

The approach with the stack allocator is to manage memory in a stack-like fashion following the LIFO principle.

As with an arena allocator, an offset into the memory block will be stored and will be moved forwards on every
allocation. The difference is that the offset can be also be moved backwards when memory is freed. With an arena
you could only free all the memory all at once.

## Basic Logic

The offset of the previous allocation needs to be tracked. This is required in order to free memory on a per-allocation
basis. One approach is to store a header which stores information about that allocation. This header means that the
allocator can know how far back it should move the offset to free that memory.

To allocate some memory from the stack allocator, it is as simple as moving the offset forward whilst accounting for
the header. The allocation has time complexity of **O(1)**.

To free a block, the header that is stored before the block of memory can be read in order to move the offset backwards.
Freeing of this memory has time complexity of **O(1)**.

## Header Storage

There are numerous approaches to stack allocators which store different data. There are three main approaches:

- Store the padding from the previous offset, also known as a *loose stack* or *small stack*.
- Store the previous offset.
- Store the size of the allocation (useful when querying the size of an allocation dynamically).

This approaches can be combined and mixed.

## Implementation of the Loose/Small Stack Allocator

Apart from the ability to free memory on a per-allocation basis, the stack allocator acts like an arena allocator.
The stack allocator will have the following procedures:

- `stack_init` initialize the stack a pre-allocated memory buffer.
- `stack_alloc` increments the offset to indicate the current buffer offset whilst taking into account the allocation header.
- `stack_free` frees the memory passed to it and decrements the offset to free that memory.
- `stack_resize` if possible resizes, otherwise calls `stack_alloc`.
- `stack_free_all` is used to free all the memory within the allocator by setting the buffer offsets to zero.

### Data Structures

The small stack data structure contains the same information as an arena.

```C
typedef struct Stack Stack;
struct Stack {
    unsigned char *buf;
    size_t buf_len;
    size_t offset;
};
```

The allocation header for this stack implementation uses an integer to encode the padding.

```C
typedef struct Stack_Allocation_Header Stack_Allocation_Header;
struct Stack_Allocation_Header {
    uint8_t padding;
};
```

The padding stores the amount of bytes that has to be placed before the header in order to have new allocation correctly aligned.

**NOTE**: Storing the padding as a byte does limit the maximum alignment that can be used with this stack allocator to
128 bytes. To calculate the maximum alignment that the padding can be used for, use this equation:

```
Maximum Alignment in Bytes = 2 ^ ((8 * sizeof(padding)) - 1)
```

### Init

The `stack_init` procedure just initializes the parameters for the given stack.



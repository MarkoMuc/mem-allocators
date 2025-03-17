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

### Alloc

Unlike an arena, stack allocator requires a header alongside the allocation. The `calc_padding_with_header` procedure
is similar to the `align_forward` which is used in with arena allocator, however is determines how much space is needed
with respect to the header and the requested alignment. In the header, the amount of padding needs to be stored and the
address after that header needs to be returned.

### Free

For `stack_free`, the pointer passed needs to be checked as to whether is valid (i.e. it was allocated by this allocator).
If it is valid, this means it is possible to acquitre the header of this allocation. Using a little pointer arithmetic,
we can reset the offset to the allocation previous to the passed pointer.

### Resize

Resizing the allocation is sometimes useful in a stack allocator. As we don't store the previous offset for this
particular version, we will just reallocate new memory if there needs to be a change in allocation size.

### Free All

This is used to free all the memory within the allocator, by setting the buffer offset to zero. This is useful for
when you want to reset on a per cycle/frame basis.

## Improving the Stack Allocator

The small stack allocator does not enforce the LIFO principles for frees. It allows the user to free any block of memory
in any order but frees everything that was allocated after it. In order to enforce the LIFO principles, data about
the previous offset needs to be stored in the header and the general data structure.

```C
struct Stack_Allocation_Header {
    size_t prev_offset;
    size_t padding;
};

struct Stack {
    unsigned char *buf;
    size_t buf_len;
    size_t prev_offset;
    size_t curr_offset;
};
```

This new header is a lot larger compared to the padding approach, but it does mean the LIFO for frees can be enforced.
There only needs to be a few adjustments to the code.

## Comments and Conclusion

If you do not want the LIFO to be enforced, it can be better to just use the `Temp_Arena_Memory` constructor instead.

You can extend the stack allocator even further by having two different offsets:

- One that starts at the beginning and increments forwards.
- One that starts at the end and increments backwards.

This is called a **double-ended stack** and allows for the maximization of memory usage whilst keeping fragmentation
extremely low (as long as the offset never overlap).

## Double-Ended Stack Allocators

Double ended stack allocators have two offsets, one at the top and one at the bottom of the stack. This allows for
allocating on both ends of the stack. This stack allocator uses memory more efficiently by allowing a trade-off to
occur between the memory usage at the bottom and the memory usage at the top of the stack. As a result we can end
up with less memory fragmentation.

#### How does this structure help with memory fragmentation?

The benefit comes from lifetimes of various objects. Normally we separate objects by their lifetimes, keeping those
with a longer lifetime on one side and those with a shorter (normally temporary lifetime) on the other end.
This allows us to either free all the temporary objects, which couldn't be done in a single ended stack since it follows
the LIFO principle. In case freeing cannot be done, e.g. because objects on the other end depend on this one,
we can still benefit by keeping the objects separate, since there tends to be some sort of spatial and temporal locality
benefit.

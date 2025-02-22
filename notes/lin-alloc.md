# Linear Allocation

## Sources

- [gingerBill Memory Allocation Strategies - Part 2](https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/).

## Basic Logic

A linear allocator is the simplest and allocates memory linearly. It is also also known an **arena allocator** or **region-based allocator**.

The arena's logic requires only an offset (or pointer) to state the end of the last allocation.

Allocating some memory from the arena, is as simple as moving the offset forward. This has complexity of **O(1)**.

The memory is usally freed all at once.

## Basic Implementation

The basic implementation just allocates the required memory, if possible, zeros out the memory and returns the pointer.

This basic implementation does not take into account memory alignment.

## Memory Alignment

Modern computer architectures always read memory at its "word size". Some do permit reading unaligned memory, but this then takes multiple reads. The resulting memory access may then be much slower than an aligned memory access.

## Aligning a Memory Address

Aligning memory usually means aligned by a power of two. This means we need to check if an address is aligned by two.

To align a memory address to the specified alignment is simple modulo arithmetic. You are looking to find how many bytes forward you need to go in order for the memory address is a multiple of the specified alignment.

```C
    p = ptr;
    a = (uintptr_t)align;
    // Same as (p % a) but faster as 'a' is a power of two
    modulo = p & (a-1);

    if (modulo != 0) {
        // If 'p' address is not aligned, push the address to the
        // next value which is aligned
        p += a - modulo;
    }
```

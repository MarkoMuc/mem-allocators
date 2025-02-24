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

### Aligning a Memory Address

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

Following is an example of an arena structure. The whole implementation can be found in `../lin-alloc/`.

```C
typedef struct Arena Arena;
struct Arena {
	unsigned char *buf;
	size_t         buf_len;
	size_t         prev_offset;
	size_t         curr_offset;
};
```

## Implementing The Rest

Apart from just the allocating function, we also need to implement initializing, freeing, resizing, etc.:

- `arena_init` initializes the arena with a pre-allocated memory buffer.
- `arena_alloc` increments the current buffer offset.
- `arena_resize` if the allocation being resized was the previously performed allocation, the same pointer will be returned and the buffer offset is changed. Otherwise, `arena_alloc` will be called instead.
- `arena_free_all` is used to free all the memory within the allocator, by setting offsets to zero.

## Using The Allocator

The allocator needs some backing memory, where the allocating are performed. The backing memory can come from either `malloc` or just an array.

```C
unsigned char backing_buffer_arr[256];
Arena a = {0};
arena_init(&a, backing_buffer_arr, 256);

void *backing_buffer_dyn = malloc(256);
Arena b = {0};
arena_init(&b, backing_buffer_dyn, 256);
```

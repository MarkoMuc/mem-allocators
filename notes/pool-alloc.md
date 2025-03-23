# Pool Allocators

## Sources

- [gingerBill Memory Allocation Strategies](https://www.gingerbill.org/article/2019/02/15/memory-allocation-strategies-004/).
- [Free lists](https://en.wikipedia.org/wiki/Free_list)

## Pool-Based Allocation

A pool allocator is differs from the other linear and stack allocators, by splitting the backing buffer into **chunks**
of equal size and keeps track of which of the chunks are free. When an allocation is wanted, a free chunk is given. When
a chunk is wanted to be freed, it adds that chunk to the list of free chunks.

Pool allocators are extremely useful when you need to allocate chunks of memory of the same size which are created and
destroyed dynamically, especially in a random order. Pools provide very little fragmentation and allocate/free in
constant time **O(1)**.

Poll allocators are usually used to allocate groups of things at once which share the same lifetime.

## Basic Logic

A pool allocator takes a backing buffer and divides that buffer into pools/chunks/slots/bins of all the same size.

``number of chunks = Memory block size / chunk size``

## Free Lists

A free list is a data structure that internally stores a linked-list of the free slots/chunks within the memory buffer.
The nodes of the list are stored in-place as this means that there does not need to be another data structure
(like an array, list, etc.) to keep track of the free slots. The data is **only** stored **within** the backing buffer
of the pool allocator.

The general approach is to store a header at the beginning (not before the chunk like with the stack allocator) of
the chunk which **points** to the next available free chunk. If there is no available chunk it points to `NULL`.

## Allocate and Free

To allocate a chunk, just pop off the head from the free list.

**Note**: The free list does not need to be ordered as its order is determined by how chunks are allocated and freed.

To free a chunk, just push the freed chunk as the head of the free list.

Both operations have the memory complexity of **O(1)**.

## Implementation

The pool allocator requires less code than the arena and stack allocator as there is no logic required for different
sized/aligned allocations and resize allocations. The complete pool allocator will have the following procedures:

- `pool_init` initialises the pool with a pre-allocated memory buffer.
- `pool_alloc` pops off the head from the free list.
- `pool_free` pushes on the freed chunk as the head of the free list.
- `pool_free_all` pushes every chunk in the pool onto the free list.

## Data Structures

The following is the `Pool` allocator, it contains a backing buffer, the size of each chunk, and the head to the free
list

```C
typedef struct Pool Pool;
struct Pool {
	unsigned char *buf;
	size_t buf_len;
	size_t chunk_size;

	Pool_Free_Node *head;
};
```

Each node in the free list just contains a pointer to the next free chunk, which could be `NULL` if is it the *tail*.

```C
typedef struct Pool_Free_Node Pool_Free_Node;
struct Pool_Free_Node {
	Pool_Free_Node *next;
};
```

## Init

Initialling a pool is pretty simple however, because each chunk has the same size and alignment, this logic
can be done now rather than later.

## Alloc

The `pool_alloc` procedure is a lot simpler than other allocators as each chunk has the same size and alignment thus
these parameters do not need to be passed to the procedure. The latest free chunk from the free list is popped off
and is then used as the new allocation.

## Free

Freeing an allocation is pretty much the opposite of an allocation. The chunk to be freed is pushed onto the free list.

## Free All

Freeing all the memory is equivalent of pushing all the chunks onto the free list.

## Conclusion

The pool allocator is very useful allocator for when you need to allocate things in *chunks* and the things within these
chunks share the same lifetime.

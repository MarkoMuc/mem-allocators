# Free List Allocators

## Sources

- [gingerBill Memory Allocation Strategies](https://www.gingerbill.org/article/2021/11/30/memory-allocation-strategies-005/).
- [Linked List](https://en.wikipedia.org/wiki/Linked_list).
- [Red-black tree](https://en.wikipedia.org/wiki/Red%e2%80%93black_tree).

## Free List Based Allocation

Pool allocator splits the backing buffer into equal chunks and keeps track of which of the chunks are free. Pools are
fast allocators that allow for *out of order free* in constant time, whilst keeping very little fragmentation. The main
restriction is that every memory allocation must be of the same size.

A free list is a general purpose allocator, that does not impose any restrictions. It allows allocations and
deallocations to be our of order and of any size. Due to its nature, the allocator's performance is not as good as linear
allocators.

There are two main approaches to implement a free list allocator:

1. Linked list.
2. Red-black tree.

## Link List Approach

We will use a linked list to store the address of free contiguous block in the memory along with its size. When the user request memory, it searches
in the linked list for a block where the data can fit. It then removes the element from the list and places an allocation header (required for free)
just before the data.

For freeing memory, we recover the allocation header (stored before the allocation) to know the size of the block we want
to free. Once that block has been freed, it is inserted into the linked list, and then we try to *coalescence*
contiguous blocks of memory together to create larger blocks.

## Linked List Free List Implementation

The following implementation does provide some constraints on the size and alignment of requested allocations with this
particular allocator. The minimum size of an allocation must be at least the size of the free list node data structure,
and the alignment has the similar requirements.

### Data Structures

Following data structures are needed for a linked list allocator.

```C
typedef struct Free_List_Allocation_Header Free_List_Allocation_Header;
struct Free_List_Allocation_Header {
    size_t block_size;
    size_t padding;
};

typedef struct Free_List_Node Free_List_Node;
struct Free_List_Node {
    Free_List_Node *next;
    size_t block_size;
};

typedef Placement_Policy Placement_Policy;
enum Placement_Policy {
    Placement_Policy_Find_First,
    Placement_Policy_Find_Best
};

typedef struct Free_List Free_List;
struct Free_List {
    void *  data;
    size_t  size;
    size_t  used;

    Free_List_Node * head;
    Placement_Policy policy;
};
```

### Allocation

TO allocate a block of memory, we need to find a block that fits our data. After we find a block in the linked list of
free memory blocks, we remove it. Finding the first block is called a *first-fit* placement policy as it stops at the
*first* block which fits the requested memory size. Another placement policy is called the *best-fit* which looks for
a free block of memory which is the smallest available which fits the memory size. The latter option reduces memory
fragmentation within the allocator.

The algorithm has a time complexity of **O(N)**, where **N** is the number of free block in the free list.

### Free and Coalescence

When freeing a memory block, we first need to retrieve the allocation header and that memory block to be treated as a
free memory block now. We then need to iterate across the linked list of free memory blocks until we get to the right
position in memory order (as the link list is sorted), and then insert new at that position. This can be achieved by
looking at the previous and next nodes in the list, since they are already sorted by address.

We also want to coalescence any free memory blocks which are contiguous. When we were iterating across linked list we
had to store both the previous and next free nodes, this means that we may be able to merge these block together if
possible.

This algorithm has a time complexity of **O(N)**, where **N** is the number of free blocks in the free list.

### Utilities

We also add general utilities needed for free list insertion, removal and calculating the padding required for the
header.

## Red-Black Tree Approach

The red-black tree improves the speed at which allocations and deallocations can be done. In the linked list, any
operations made is needed to be iterated across linearly **O(N)**. A red-black reduces its time complexity to
**O(log(N))**, whilst keeping the space complexity relatively low. And as a consequence of this data-structure approach,
a *best-fit* algorithm may be taken always.

The minor increase in space complexity is due to using a sorted doubly linked list, but as a consequence, it allows
coalescence operations in **O(1)** time.

This implementation is a common aspect in many `malloc` implementations, but note that most `malloc`s utilize multiple
different memory allocation strategies that complement each other.

## Conclusion

The free list allocator is a very useful allocator for when you need a general purpose allocator that requires
allocations of arbitrary size and out of order deallocations.

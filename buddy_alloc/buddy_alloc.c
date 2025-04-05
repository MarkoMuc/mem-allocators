#include "buddy_alloc.h"

inline bool is_power_of_two(uintptr_t x) {
    return (x & (x-1)) == 0;
}

Buddy_Block *buddy_block_next(Buddy_Block *block) {
    return (Buddy_Block *)((char *)block + block->size);
}

Buddy_Block *buddy_block_find_best(Buddy_Block *head, Buddy_Block *tail, size_t size) {
    Buddy_Block *best_block = NULL;
    Buddy_Block *block = head; // left buddy
    Buddy_Block *buddy = buddy_block_next(block); // right buddy

    if(buddy == tail && block->is_free) {
        return buddy_block_split(block, size);
    }

    while(block < tail && buddy < tail) {
        if(block->is_free && buddy->is_free && block->size == buddy->size) {
            // This part is an optimization that reduces fragmentation, otherwise it is not needed
            block->size <<= 1;

            if(size <= block->size && (best_block == NULL || block->size <= best_block->size)) {
                best_block = block;
            }

            block = buddy_block_next(buddy);
            if(block < tail) {
                // Delays the buddy block for the next iteration
                buddy = buddy_block_next(block);
            }
            continue;
        }

        if(block->is_free && size <= block->size &&
            (best_block == NULL || block->size <= best_block->size)) {
            best_block = block;
        }

        if(buddy->is_free && size <= buddy->size &&
            (best_block == NULL || buddy->size < best_block->size)) {
            // If each buddy are the same size, then it makes more sense to pick
            // the buddy as it "bounces around" less
            best_block = buddy;
        }

        if(block->size <= buddy->size) {
            block = buddy_block_next(buddy);
            if(block < tail) {
                // Delay the buddy block for the next iteration
                buddy = buddy_block_next(block);
            }
        } else {
            // Buddy was split into smaller block
            block = buddy;
            buddy = buddy_block_next(buddy);
        }
    }

    if(best_block != NULL) {
        // This handles the case when the best_block is also the perfect fit
        return buddy_block_split(best_block, size);
    }

    return NULL;
}

void buddy_block_init(Buddy_Allocator *b, void *data, size_t size, size_t alignment) {
    assert(data != NULL);
    assert(is_power_of_two(size) && "size is not power-of-two");
    assert(is_power_of_two(alignment) && "alignment is not power-of-two");

    // The minimum alignment depends on the size of the Buddy_block header
    assert(is_power_of_two(sizeof(Buddy_Block) == 0));
    if(alignment < sizeof(Buddy_Block)) {
        alignment = sizeof(Buddy_Block);
    }
    assert((uintptr_t)data % alignment == 0 && "data is not aligned to minimum alignment");

    b->head = (Buddy_Block *)data;
    b->head->size = size;
    b->head->is_free = true;

    b->tail = buddy_block_next(b->head);

    b->alignment = alignment;
}

Buddy_Block *buddy_block_split(Buddy_Block *block, size_t size) {
    if(block != NULL && size != 0) {
        while(size < block->size) {
            size_t sz = block->size >> 1;
            block->size = sz;
            block = buddy_block_next(block);
            block->size = sz;
            block->is_free = true;
        }

        if(size <= block->size) {
            return block;
        }
    }

    return NULL;
}

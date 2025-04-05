#ifndef STD_ASSERT
#define STD_ASSERT
#include <assert.h>
#endif

#ifndef STD_BOOL
#define STD_BOOl
#include <stdbool.h>
#endif

#ifndef STD_INT
#define STD_INT
#include <stdint.h>
#endif

#ifndef STD_LIB
#define STD_LIB
#include <stdlib.h>
#endif

#ifndef STD_STRING
#define STD_STRING
#include <string.h>
#endif

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2*sizeof(void *))
#endif

typedef struct Buddy_Block Buddy_Block;
struct Buddy_Block {
    size_t size;
    bool is_free;
};

typedef struct Buddy_Allocator Buddy_Allocator;
struct Buddy_Allocator {
    Buddy_Block *head;
    Buddy_Block *tail;
    size_t alignment;
};

bool is_power_of_two(uintptr_t x);
Buddy_Block *buddy_block_next(Buddy_Block *block);
size_t buddy_block_size_required(Buddy_Allocator *b, size_t size);

void buddy_block_coalescence(Buddy_Block *head, Buddy_Block *tail);
Buddy_Block *buddy_block_find_best(Buddy_Block *head, Buddy_Block *tail, size_t size);
void buddy_block_init(Buddy_Allocator *b, void *data, size_t size, size_t alignment);
Buddy_Block *buddy_block_split(Buddy_Block *block, size_t size);


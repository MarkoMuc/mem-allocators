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

typedef struct Arena Arena;

bool is_power_of_two(uintptr_t x);
uintptr_t align_forward(uintptr_t ptr, size_t align);
void *arena_alloc_align(Arena *a, size_t size, size_t align);
void *arena_resize_align(Arena *a, void *old_memory, size_t old_size, size_t new_size, size_t align);

void arena_init(Arena *a, void *backing_buffer, size_t backing_buffer_len);
void *arena_alloc(Arena *a, size_t size);
void *arena_resize(Arena *a, void *old_memory, size_t old_size, size_t new_size);
void arena_free_all(Arena *a);

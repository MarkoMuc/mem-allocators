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

typedef struct Pool_Free_Node Pool_Free_Node;
struct Pool_Free_Node {
	Pool_Free_Node *next;
};

typedef struct Pool Pool;
struct Pool {
	unsigned char *buf;
	size_t buf_len;
	size_t chunk_size;

	Pool_Free_Node *head;
};


bool is_power_of_two(uintptr_t x);
uintptr_t align_forward_uinptr(uintptr_t ptr, uintptr_t align);
size_t align_forward_size(size_t ptr, size_t align);

void *pool_alloc(Pool *p);
void pool_free(Pool *p, void *ptr);
void pool_free_all(Pool *p);
void pool_init(Pool *p, void *backing_buffer, size_t backing_buffer_length, size_t chunk_size, size_t chunk_alignment);

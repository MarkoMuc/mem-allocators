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

enum StackSide { STACK_FRONT, STACK_END };

typedef struct Stack Stack;
typedef struct Stack_Alloc_Header Stack_Alloc_Header;

struct Stack {
    unsigned char *buf;
    size_t buf_len;
    size_t start_offset;
    size_t start_prev_offset;
    size_t end_offset;
    size_t end_prev_offset;
};

struct Stack_Alloc_Header {
    size_t prev_offset;
    size_t padding;
};

bool is_power_of_two(uintptr_t x);
size_t calc_padding_with_header(uintptr_t ptr, uintptr_t alignment, size_t header_size);
void *stack_alloc_align(Stack *s, size_t size, enum StackSide side, size_t alignment);
void *stack_resize_align(Stack *s, void *ptr, size_t old_size, size_t new_size, enum StackSide side, size_t alignment);

void *stack_alloc_end(Stack *s, size_t size);
void *stack_alloc_front(Stack *s, size_t size);
void *stack_resize_end(Stack *s, void *ptr, size_t old_size, size_t new_size);
void *stack_resize_front(Stack *s, void *ptr, size_t old_size, size_t new_size);
void stack_free(Stack *s, void *ptr, enum StackSide side);
void stack_free_all(Stack *s);
void stack_free_end(Stack *s, void *ptr);
void stack_free_front(Stack *s, void *ptr);
void stack_init(Stack *s, void *backing_buffer, size_t backing_buffer_length);

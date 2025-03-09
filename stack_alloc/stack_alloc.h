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

typedef struct Stack Stack;
typedef struct Stack_Allocation_Header Stack_Allocation_Header;

typedef struct Strict_Stack Strict_Stack;
typedef struct Strict_Stack_Alloc_Header Strict_Stack_Alloc_Header;

bool is_power_of_two(uintptr_t x);
size_t calc_padding_with_header(uintptr_t ptr, uintptr_t alignment, size_t header_size);
void *stack_alloc_align(Stack *s, size_t size, size_t alignment);
void *stack_resize_align(Stack *s, void *ptr, size_t old_size, size_t new_size, size_t alignment);
void *strict_stack_alloc_align(Strict_Stack *s, size_t size, size_t alignment);
void *strict_stack_resize_align(Strict_Stack *s, void *ptr, size_t old_size, size_t new_size, size_t alignment);

void *stack_alloc(Stack *s, size_t size);
void *stack_resize(Stack *s, void *ptr, size_t old_size, size_t new_size);
void stack_free(Stack *s, void *ptr);
void stack_free_all(Stack *s);
void stack_init(Stack *s, void *backing_buffer, size_t backing_buffer_length);

void *strict_stack_alloc(Strict_Stack *s, size_t size);
void *strict_stack_resize(Strict_Stack *s, void *ptr, size_t old_size, size_t new_size);
void strict_stack_free(Strict_Stack *s, void *ptr);
void strict_stack_free_all(Strict_Stack *s);
void strict_stack_init(Strict_Stack *s, void *backing_buffer, size_t backing_buffer_length);

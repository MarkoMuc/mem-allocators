#include "double_stack_alloc.h"

inline bool is_power_of_two(uintptr_t x) {
    return (x & (x - 1)) == 0;
}

size_t calc_padding_with_header(uintptr_t ptr, uintptr_t alignment, size_t header_size) {
    uintptr_t p, a, modulo, padding, needed_space;

    assert(is_power_of_two(alignment));

    p = ptr;
    a = alignment;
    modulo = p & (a - 1);

    padding = 0;

    if(modulo != 0) {
        padding = a - modulo;
    }
    needed_space = (uintptr_t) header_size;

    if(padding < needed_space) {
        needed_space -= padding;
        if((needed_space & (a-1)) != 0) {
            padding += a * (1 + (needed_space/a));
        } else {
            padding += a * (needed_space/a);
        }
    }

    return (size_t)padding;
}

void *stack_alloc_align(Stack *s, size_t size, bool side, size_t alignment) {
    uintptr_t curr_addr, next_addr;
    size_t padding, prev_offset;
    Stack_Alloc_Header *header;

    if(alignment > 128) {
        alignment = 128;
    }

    curr_addr = (uintptr_t)s->buf + (uintptr_t)(side == STACK_FRONT? s->start_offset : s->end_offset);
    padding = calc_padding_with_header(curr_addr, alignment, sizeof(Stack_Alloc_Header));

    if((side == STACK_FRONT && s->start_offset + padding + size >= s->end_offset)
        || (side == STACK_END && s->end_offset - padding - size <= s->start_offset)) {
        return NULL;
    }

    if(side == STACK_FRONT) {
        prev_offset = s->start_offset;
        s->start_offset += padding; 
        next_addr = curr_addr + (uintptr_t)padding;
        header = (Stack_Alloc_Header *)(next_addr - sizeof(Stack_Alloc_Header));
        s->start_offset += size;
    } else {
        prev_offset = s->end_offset;
        s->end_offset -= padding; 
        next_addr = curr_addr - (uintptr_t)padding;
        header = (Stack_Alloc_Header *)(next_addr + sizeof(Stack_Alloc_Header));
        s->end_offset -= size;
    }

    header->padding = (uint8_t)padding;
    header->prev_offset = prev_offset;

    return memset((void*)next_addr, 0, size);
}

void *stack_resize_align(Stack *s, void *ptr, size_t old_size, size_t new_size, size_t alignment);

void *stack_alloc_front(Stack *s, size_t size) {
    return stack_alloc_align(s, size, STACK_FRONT, DEFAULT_ALIGNMENT);
}

void *stack_alloc_end(Stack *s, size_t size) {
    return stack_alloc_align(s, size, STACK_END, DEFAULT_ALIGNMENT);
}

void *stack_resize(Stack *s, void *ptr, size_t old_size, size_t new_size);
void stack_free(Stack *s, void *ptr);

void stack_free_all(Stack *s) {
    s->start_offset = 0;
    s->end_offset = s->buf_len - 1;
}

void stack_init(Stack *s, void *backing_buffer, size_t backing_buffer_length) {
    s->buf = (unsigned char *)backing_buffer;
    s->buf_len = backing_buffer_length;
    s->start_offset = 0;
    s->end_offset = backing_buffer_length - 1;
}

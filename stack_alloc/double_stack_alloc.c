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

void *stack_alloc_align(Stack *s, size_t size, enum StackSide side, size_t alignment) {
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

void *stack_resize_align(Stack *s, void *ptr, size_t old_size, size_t new_size, enum StackSide side, size_t alignment) {
    if (!ptr) {
        return stack_alloc_align(s, new_size, side, alignment);
    } else if (new_size == 0) {
        stack_free(s, ptr, side);
        return NULL;
    } else {
        uintptr_t start, end, curr_addr, prev_offset;
        size_t min_size = old_size < new_size? old_size : new_size;
        Stack_Alloc_Header *header;
        void *new_ptr;

        start = (uintptr_t)s->buf;
        end = start + (uintptr_t)s->buf_len;
        curr_addr = (uintptr_t)ptr;

        if(!(start <= curr_addr && curr_addr < end)) {
            assert(0 && "Out of bounds memory address passed to stack allocator (resize)");
            return NULL;
        }

        if (old_size == min_size) {
            return ptr;
        }

        header = (Stack_Alloc_Header *)(curr_addr - sizeof(Stack_Alloc_Header));
        prev_offset = (size_t)(curr_addr + ((side == STACK_FRONT? -1 : 1) * header->padding) - start);

        const size_t next_addr_offset = (size_t) curr_addr + ((side == STACK_FRONT? 1 : -1) * old_size) - (size_t) start;
        if ((side == STACK_FRONT && next_addr_offset != s->start_offset) ||
            (side == STACK_END && next_addr_offset != s->end_offset)) {
            new_ptr = stack_alloc_align(s, new_size, side, alignment);
        } else {
            new_ptr = (void *)(curr_addr + (uintptr_t)new_size);
            if (side == STACK_FRONT) {
                s->start_offset = (size_t)(curr_addr + new_size - start);
            } else {
                s->end_offset = (size_t)(curr_addr - new_size - start);
            }
        }

        memmove(new_ptr, ptr, min_size);
        return new_ptr;
    }
}

void *stack_alloc_front(Stack *s, size_t size) {
    return stack_alloc_align(s, size, STACK_FRONT, DEFAULT_ALIGNMENT);
}

void *stack_alloc_end(Stack *s, size_t size) {
    return stack_alloc_align(s, size, STACK_END, DEFAULT_ALIGNMENT);
}

void *stack_resize_front(Stack *s, void *ptr, size_t old_size, size_t new_size) {
    return stack_resize_align(s, ptr, old_size, new_size, STACK_FRONT, DEFAULT_ALIGNMENT);
}

void *stack_resize_end(Stack *s, void *ptr, size_t old_size, size_t new_size) {
    return stack_resize_align(s, ptr, old_size, new_size, STACK_END, DEFAULT_ALIGNMENT);
}

void stack_free(Stack *s, void *ptr, enum StackSide side) {
    if(ptr != NULL) {
        uintptr_t start, end, curr_addr;
        Stack_Alloc_Header *header;
        size_t prev_offset;

        start = (uintptr_t)s->buf;
        end = start + (uintptr_t)s->buf_len;
        curr_addr = (uintptr_t)ptr;

        if(!(start <= curr_addr && curr_addr < end)) {
            assert(0 && "Out of bounds memory address passed to stack allocator (free)");
            return;
        }

        if((side == STACK_FRONT && curr_addr >= start+(uintptr_t)s->start_offset) ||
            (side == STACK_END && curr_addr <= start+(uintptr_t)s->end_offset)) {
            // Double free
            return;
        }

        header = (Stack_Alloc_Header *)(curr_addr + sizeof(Stack_Alloc_Header));
        prev_offset = (size_t)(curr_addr - (uintptr_t)header->padding - start);

        if(prev_offset != s->prev_offset) {
            assert(0 && "Out of order stack allocator free");
            return;
        }

        s->curr_offset = s->prev_offset;
        s->prev_offset = header->prev_offset;
    }
}

void stack_free_front(Stack *s, void *ptr) {
    stack_free(s, ptr, STACK_FRONT);
}

void stack_free_end(Stack *s, void *ptr) {
    stack_free(s, ptr, STACK_END);
}

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

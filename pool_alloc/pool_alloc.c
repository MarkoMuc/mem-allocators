#include "pool_alloc.h"

inline bool is_power_of_two(uintptr_t x) {
    return (x & (x-1)) == 0;
}

uintptr_t align_forward_uinptr(uintptr_t ptr, uintptr_t align) {
    uintptr_t a, p, modulo;

    assert(is_power_of_two(align));

    a = align;
    p = ptr;
    modulo = p & (a-1);

    if(modulo != 0) {
        p += a - modulo;
    }

    return p;
}

size_t align_forward_size(size_t ptr, size_t align) {
    size_t a, p, modulo;

    assert(is_power_of_two((uintptr_t)align));

    a = align;
    p = ptr;
    modulo = p & (a-1);

    if(modulo != 0) {
        p += a - modulo;
    }

    return p;
}

void *pool_alloc(Pool *p) {
    Pool_Free_Node *node = p->head;

    if(node == NULL) {
        assert(0 && "Pool allocator has no free memory");
        return NULL;
    }

    p->head = p->head->next;

    return memset(node, 0, p->chunk_size);
}

void pool_free(Pool *p, void *ptr) {
    Pool_Free_Node *node;

    void *start = p->buf;
    void *end = &p->buf[p->buf_len];

    if(ptr == NULL) {
        return;
    }

    if(!(start <= ptr && ptr < end)) {
        assert(0 && "Memory is out of bounds of the buffer in this pool");
        return;
    }

    node = (Pool_Free_Node *)ptr;
    node->next = p->head;
    p->head = node;
}

void pool_free_all(Pool *p) {
    size_t chunk_count = p->buf_len / p->chunk_size;

    for (size_t i = 0; i < chunk_count; i++) {
        void *ptr = &p->buf[i * p->chunk_size];
        Pool_Free_Node *node = (Pool_Free_Node *) ptr;
        node->next = p->head;
        p->head = node;
    }
}

void pool_init(Pool *p, void *backing_buffer, size_t backing_buffer_length, size_t chunk_size, size_t chunk_alignment) {
    uintptr_t initial_start = (uintptr_t) backing_buffer;
    uintptr_t start = align_forward_uinptr(initial_start, (uintptr_t)chunk_alignment);
    backing_buffer_length -= (size_t)(start - initial_start);

    chunk_size = align_forward_size(chunk_size, chunk_alignment);

    assert(chunk_size >= sizeof(Pool_Free_Node) && "Chunk size is too small.");
    assert(backing_buffer_length >= chunk_size && "Backing buffer length is smaller than the actual size.");

    p->buf = (unsigned char*)backing_buffer;
    p->buf_len = backing_buffer_length;
    p->chunk_size = chunk_size;
    p->head = NULL;

    pool_free_all(p);
}

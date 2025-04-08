#include "list_alloc.h"

inline bool is_power_of_two(uintptr_t x) {
    return (x & (x-1)) == 0;
}

size_t calc_padding_with_header(uintptr_t ptr, uintptr_t alignment, size_t header_size) {
    uintptr_t p, a, modulo, padding, needed_space;

    assert(is_power_of_two(alignment));

    p = ptr;
    a = alignment;
    modulo = p & (a - 1);

    padding = 0;
    needed_space = 0;

    if(modulo != 0) {
        padding = a - modulo;
    }

    needed_space = (uintptr_t)header_size;

    if(padding < needed_space) {
        needed_space -= padding;

        if((needed_space & (a-1)) != 0) {
            padding += a * (1+(needed_space/a));
        } else {
            padding += a * (needed_space/a);
        }
    }

    return (size_t)padding;
}

void free_list_free(Free_List *fl, void *ptr) {
    Free_List_Alloc_Header *header;
    Free_List_Node *free_node;
    Free_List_Node *node;
    Free_List_Node *prev_node = NULL;

    if(ptr == NULL) {
        return;
    }

    header = (Free_List_Alloc_Header *)((char *)ptr - sizeof(Free_List_Alloc_Header));
    free_node = (Free_List_Node *)header;
    free_node->block_size = header->block_size + header->padding;
    free_node->next = NULL;

    node = fl->head;
    while(node != NULL) {
        if((uintptr_t)ptr < (uintptr_t)node) {
            free_list_node_insert(&fl->head, prev_node, free_node);
            break;
        }
        prev_node = node;
        node = node->next;
    }

    fl->used -= free_node->block_size;

    free_list_coalescence(fl, prev_node, free_node);
}

void *free_list_alloc(Free_List *fl, size_t size, size_t alignment) {
    size_t padding = 0;
    Free_List_Node *prev_node = NULL;
    Free_List_Node *node = NULL;
    size_t alignment_padding, required_space, remaining;
    Free_List_Alloc_Header *header_ptr;

    if(size < sizeof(Free_List_Node)) {
        size = sizeof(Free_List_Node);
    }

    if(alignment < 0) {
        alignment = 8;
    }

    if(fl->policy == Placement_Policy_Find_Best) {
        node = free_list_find_best(fl, size, alignment, &padding, &prev_node);
    } else {
        node = free_list_find_first(fl, size, alignment, &padding, &prev_node);
    }

    if(node == NULL) {
        assert(0 && "Free list has no free memory");
        return NULL;
    }

    alignment_padding = padding - sizeof(Free_List_Alloc_Header);
    required_space = size + padding;
    remaining = node->block_size - required_space;

    if(remaining > 0) {
        Free_List_Node *new_node = (Free_List_Node*)((char *)node + required_space);
        new_node->block_size = remaining;
        free_list_node_insert(&fl->head, node, new_node);
    }

    free_list_node_remove(&fl->head, prev_node, node);

    header_ptr = (Free_List_Alloc_Header *)((char *)node + alignment_padding);
    header_ptr->block_size = required_space;
    header_ptr->padding = alignment_padding;

    fl->used += required_space;

    return (void *)((char *)header_ptr + sizeof(Free_List_Alloc_Header));
}

void free_list_coalescence(Free_List *fl, Free_List_Node *prev_node, Free_List_Node *free_node) {
    if(free_node->next != NULL && (void *)((char *)free_node + free_node->block_size) == free_node->next) {
        free_node->block_size += free_node->next->block_size;
        free_list_node_remove(&fl->head, prev_node, free_node->next);
    }

    if(prev_node->next != NULL && (void *)((char *)prev_node + prev_node->block_size) == free_node) {
        prev_node->block_size += free_node->next->block_size;
        free_list_node_remove(&fl->head, prev_node, free_node);
    }
}

void free_list_free_all(Free_List *fl) {
    fl->used = 0;
    Free_List_Node *first_node = (Free_List_Node *)fl->data;
    first_node->block_size = fl->size;
    first_node->next = NULL;
    fl->head = first_node;
}

void free_list_init(Free_List *fl, void *data, size_t size) {
    fl->data = data;
    fl->size = size;
    free_list_free_all(fl);
}

void *free_list_find_best(Free_List *fl, size_t size, size_t alignment, size_t *_padding, Free_List_Node **_prev_node) {
    size_t smallest_diff = ~(size_t)0;
    Free_List_Node *node = fl->head;
    Free_List_Node *prev_node = NULL;
    Free_List_Node *best_node = NULL;

    size_t padding = 0;

    while(node != NULL) {
        padding = calc_padding_with_header((uintptr_t)node, (uintptr_t)alignment, sizeof(Free_List_Alloc_Header));
        size_t required_space = size + padding;
        if(node->block_size >= required_space && (node->block_size - required_space < smallest_diff)) {
            smallest_diff = node->block_size - required_space;
            best_node = node;
        }

        prev_node = node;
        node = node->next;
    }

    if(_padding) {
        *_padding = padding;
    }

    if(_prev_node) {
        *_prev_node = prev_node;
    }

    return best_node;
}

void *free_list_find_first(Free_List *fl, size_t size, size_t alignment, size_t *_padding, Free_List_Node **_prev_node) {
    Free_List_Node *node = fl->head;
    Free_List_Node *prev_node = NULL;

    size_t padding = 0;

    while(node != NULL) {
        padding = calc_padding_with_header((uintptr_t)node, (uintptr_t)alignment, sizeof(Free_List_Alloc_Header));
        size_t required_space = size + padding;
        if(node->block_size >= required_space) {
            break;
        }
        prev_node = node;
        node = node->next;
    }

    if(_padding) {
        *_padding = padding;
    }

    if(_prev_node) {
        *_prev_node = prev_node;
    }

    return node;
}


void free_list_node_insert(Free_List_Node **phead, Free_List_Node *prev_node, Free_List_Node *new_node) {
    if(prev_node == NULL) {
        if(*phead != NULL) {
            new_node->next = *phead;
        } else {
            *phead = new_node;
        }
    } else {
        if(prev_node->next == NULL) {
            prev_node->next = new_node;
            new_node->next = NULL;
        } else {
            new_node->next = prev_node->next;
            prev_node->next = new_node;
        }
    }
}

void free_list_node_remove(Free_List_Node **phead, Free_List_Node *prev_node, Free_List_Node *del_node) {
    if(prev_node == NULL) {
        *phead = del_node->next;
    } else {
        prev_node->next = del_node->next;
    }
}

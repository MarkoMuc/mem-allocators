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

typedef struct Free_List_Alloc_Header Free_List_Alloc_Header;
struct Free_List_Alloc_Header {
    size_t block_size;
    size_t padding;
};

typedef struct Free_List_Node Free_List_Node;
struct Free_List_Node {
    Free_List_Node *next;
    size_t block_size;
};

enum Placement_Policy {
    Placement_Policy_Find_First,
    Placement_Policy_Find_Best
};
typedef enum Placement_Policy Placement_Policy;

typedef struct Free_List Free_List;
struct Free_List {
    void *data;
    size_t size;
    size_t used;

    Free_List_Node *head;
    Placement_Policy policy;
};

bool is_power_of_two(uintptr_t x);
size_t calc_padding_with_header(uintptr_t ptr, uintptr_t alignment, size_t header_size);

void *free_list_alloc(Free_List *fl, size_t size, size_t alignment);
void free_list_coalescence(Free_List *fl, Free_List_Node *prev_node, Free_List_Node *free_node);
void free_list_free(Free_List *fl, void *ptr);
void free_list_free_all(Free_List *fl);
void free_list_init(Free_List *fl, void *data, size_t size);

void free_list_node_insert(Free_List_Node **phead, Free_List_Node *prev_node, Free_List_Node *new_node);
void free_list_node_remove(Free_List_Node **phead, Free_List_Node *prev_node, Free_List_Node *del_node);

void *free_list_find_best(Free_List *fl, size_t size, size_t alignment, size_t *_padding, Free_List_Node **_prev_node);
void *free_list_find_first(Free_List *fl, size_t size, size_t alignment, size_t *_padding, Free_List_Node **_prev_node);

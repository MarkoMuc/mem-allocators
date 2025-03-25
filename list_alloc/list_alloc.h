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
    site_t padding;
};

typedef struct Free_List_Node Free_List_Node;
struct Free_List_Node {
    Free_List_Node *next;
    size_t block_size;
};

typedef enum Placement_Policy Placement_Policy;
enum Placement_Policy {
    Placement_Policy_Find_First,
    Placement_Policy_Find_Best
};

typedef struct Free_List Free_List;
struct Free_List {
    void *data;
    size_t size;
    size_t used;

    Free_List_Node *head;
    Placement_Policy policy;
}

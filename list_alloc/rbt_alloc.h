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
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif

typedef struct Free_List_Alloc_Header Free_List_Alloc_Header;
struct Free_List_Alloc_Header {
  size_t block_size;
  size_t padding;
};

enum RBT_Color { RBT_RED, RBT_BLACK };
typedef enum RBT_Color RBT_Color;

typedef struct RBT_Node RBT_Node;
struct RBT_Node {
  RBT_Node *parent;
  union {
    RBT_Node *left;
    RBT_Node *right;
    RBT_Node *children[2];
  };
  RBT_Color color;
  size_t block_size;
};

typedef struct RBT_Alloc RBT_Alloc;
struct RBT_Alloc {
  void *data;
  size_t size;
  size_t used;
  RBT_Node *head;
};

bool is_power_of_two(uintptr_t x);
size_t calc_padding_with_header(uintptr_t ptr, uintptr_t alignment,
                                size_t header_size);

void rbt_free(RBT_Alloc *rbt, void *ptr);
void rbt_free_all(RBT_Alloc *rbt);
void rbt_init(RBT_Alloc *rbt, void *data, size_t size);

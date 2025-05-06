#include "rbt_alloc.h"

inline bool is_power_of_two(uintptr_t x) { return (x & (x - 1)) == 0; }

size_t calc_padding_with_header(uintptr_t ptr, uintptr_t alignment,
                                size_t header_size) {
  uintptr_t p, a, modulo, padding, needed_space;

  assert(is_power_of_two(alignment));

  p = ptr;
  a = alignment;
  modulo = p & (a - 1);

  padding = 0;
  needed_space = 0;

  if (modulo != 0) {
    padding = a - modulo;
  }

  needed_space = (uintptr_t)header_size;

  if (padding < needed_space) {
    needed_space -= padding;

    if ((needed_space & (a - 1)) != 0) {
      padding += a * (1 + (needed_space / a));
    } else {
      padding += a * (needed_space / a);
    }
  }

  return (size_t)padding;
}

void rbt_free(RBT_Alloc *rbt, void *ptr);
void rbt_free_all(RBT_Alloc *rbt);
void rbt_init(RBT_Alloc *rbt, void *data, size_t size) {
  rbt->data = data;
  rbt->size = size;
  rbt->used = 0;

  rbt->head = (RBT_Node *)data;
  rbt->head->color = RBT_BLACK;
  rbt->head->block_size = size;
  rbt->head->parent = NULL;
  rbt->head->left = NULL;
  rbt->head->right = NULL;
}

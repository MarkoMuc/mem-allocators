#include "buddy_alloc.h"

inline bool is_power_of_two(uintptr_t x) {
    return (x & (x-1)) == 0;
}

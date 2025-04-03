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

bool is_power_of_two(uintptr_t x);

#include <assert.h>
#include <stdlib.h>
#include <math.h>

#ifndef DA_INITIAL_CAPACITY
#define DA_INITIAL_CAPACITY 32 
#endif

#ifndef DA_GROWTH_FACTOR
#define DA_GROWTH_FACTOR 1.5f
#endif

#define da_def(items_type, type_name)           \
  typedef struct {                              \
    items_type *items;                          \
    size_t count;                               \
    size_t capacity;                            \
  } type_name;                                       
#define da_init(da_type) (da_type){.items = NULL, .count = 0, .capacity = 0}
#define da_free(da) do {                        \
    if ((da)->items != NULL)                    \
      free((da)->items);                        \
} while (0)

#define da_append(da, item) do {                                        \
    if ((da)->count >= (da)->capacity) {                                \
      (da)->capacity = (da)->capacity == 0                              \
        ? DA_INITIAL_CAPACITY                                           \
        : (int)round((da)->capacity * DA_GROWTH_FACTOR);                \
      (da)->items = realloc((da)->items, (da)->capacity * sizeof(*(da)->items)); \
      assert((da)->items != NULL && "Not enough memory to allocate for the dynamic array"); \
    }                                                                   \
    (da)->items[(da)->count++] = (item);                                \
} while(0)               

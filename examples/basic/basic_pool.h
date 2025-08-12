#ifndef BASIC_POOL_H
#define BASIC_POOL_H

#include <stddef.h>

/* Call once before any pool allocations.  The BASIC runtime initializes
   the pool during startup, so standalone tools must invoke this manually
   with the desired initial size. */
void basic_pool_init (size_t size);
void *basic_pool_alloc (size_t size);
void basic_pool_reset (void);
void basic_pool_destroy (void);
void basic_pool_free (void *p);

char *basic_alloc_string (size_t len);
void *basic_alloc_array (size_t count, size_t elem_size, int clear);
void *basic_calloc (size_t count, size_t elem_size);
int basic_clear_array_pool (void *base, size_t len, size_t elem_size);

#endif /* BASIC_POOL_H */

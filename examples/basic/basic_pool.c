#include "basic_pool.h"

#include <stdlib.h>
#include <string.h>

static char *pool_base = NULL;
static size_t pool_size = 0;
static size_t pool_offset = 0;

void basic_pool_init (size_t size) {
  basic_pool_destroy ();
  pool_base = malloc (size);
  if (pool_base == NULL) {
    pool_size = 0;
    return;
  }
  pool_size = size;
  pool_offset = 0;
}

void *basic_pool_alloc (size_t size) {
  if (pool_base == NULL || pool_offset + size > pool_size) return NULL;
  void *res = pool_base + pool_offset;
  pool_offset += size;
  return res;
}

void basic_pool_reset (void) { pool_offset = 0; }

void basic_pool_destroy (void) {
  free (pool_base);
  pool_base = NULL;
  pool_size = pool_offset = 0;
}

char *basic_alloc_string (size_t len) {
  char *res = basic_pool_alloc (len + 1);
  if (res == NULL) return NULL;
  res[len] = '\0';
  return res;
}

void *basic_alloc_array (size_t count, size_t elem_size, int clear) {
  size_t total = count * elem_size;
  void *res = basic_pool_alloc (total);
  if (res != NULL && clear) memset (res, 0, total);
  return res;
}

#include "basic_pool.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
  char *data;
  size_t size;
  size_t pos;
} BasicPool;

static BasicPool pool;

static size_t align_up (size_t n) {
  size_t align = sizeof (void *);
  return (n + align - 1) & ~(align - 1);
}

void basic_pool_init (size_t size) {
  pool.data = size ? malloc (size) : NULL;
  pool.size = pool.data ? size : 0;
  pool.pos = 0;
}

void *basic_pool_alloc (size_t size) {
  size = align_up (size);
  if (pool.pos + size > pool.size) {
    size_t newsize = pool.size ? pool.size : 1024;
    while (pool.pos + size > newsize) newsize *= 2;
    char *newdata = realloc (pool.data, newsize);
    if (newdata == NULL) return NULL;
    pool.data = newdata;
    pool.size = newsize;
  }
  void *res = pool.data + pool.pos;
  pool.pos += size;
  return res;
}

void basic_pool_reset (void) { pool.pos = 0; }

void basic_pool_destroy (void) {
  free (pool.data);
  pool.data = NULL;
  pool.size = pool.pos = 0;
}

char *basic_alloc_string (size_t len) {
  char *s = basic_pool_alloc (len + 1);
  if (s != NULL) s[len] = '\0';
  return s;
}

void *basic_alloc_array (size_t count, size_t elem_size, int clear) {
  size_t n = count * elem_size;
  void *p = basic_pool_alloc (n);
  if (p != NULL && clear) memset (p, 0, n);
  return p;
}

void *basic_calloc (size_t count, size_t elem_size) {
  return basic_alloc_array (count, elem_size, 1);
}

int basic_clear_array_pool (void *base, size_t count, size_t elem_size) {
  if (base == NULL) return 1;
  size_t n = align_up (count * elem_size);
  char *p = (char *) base;
  if (pool.data != NULL && p + n == pool.data + pool.pos) {
    pool.pos -= n;
    return 1;
  }
  return 0;
}

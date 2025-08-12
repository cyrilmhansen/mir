#include "basic_pool.h"

#include <stdlib.h>
#include <string.h>

typedef struct PoolBlock {
  char *data;
  size_t size;
  size_t pos;
  struct PoolBlock *next;
} PoolBlock;

static PoolBlock *pool = NULL;

static size_t align_up (size_t n) {
  size_t align = sizeof (void *);
  return (n + align - 1) & ~(align - 1);
}

void basic_pool_init (size_t size) {
  if (pool != NULL) basic_pool_destroy ();
  if (size != 0) {
    pool = malloc (sizeof (PoolBlock));
    if (pool != NULL) {
      pool->data = malloc (size);
      if (pool->data == NULL) {
        free (pool);
        pool = NULL;
      } else {
        pool->size = size;
        pool->pos = 0;
        pool->next = NULL;
      }
    }
  } else {
    pool = NULL;
  }
}

static PoolBlock *curr_block (void) {
  PoolBlock *b = pool;
  if (b == NULL) return NULL;
  while (b->next != NULL) b = b->next;
  return b;
}

void *basic_pool_alloc (size_t size) {
  size = align_up (size);
  if (pool == NULL) basic_pool_init (1024);
  PoolBlock *b = curr_block ();
  if (b == NULL) return NULL;
  if (b->pos + size > b->size) {
    size_t newsize = b->size ? b->size * 2 : 1024;
    while (newsize < size) newsize *= 2;
    PoolBlock *nb = malloc (sizeof (PoolBlock));
    if (nb == NULL) return NULL;
    nb->data = malloc (newsize);
    if (nb->data == NULL) {
      free (nb);
      return NULL;
    }
    nb->size = newsize;
    nb->pos = 0;
    nb->next = NULL;
    b->next = nb;
    b = nb;
  }
  void *res = b->data + b->pos;
  b->pos += size;
  return res;
}

void basic_pool_reset (void) {
  for (PoolBlock *b = pool; b != NULL; b = b->next) b->pos = 0;
}

void basic_pool_destroy (void) {
  PoolBlock *b = pool;
  while (b != NULL) {
    PoolBlock *next = b->next;
    free (b->data);
    free (b);
    b = next;
  }
  pool = NULL;
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
  for (PoolBlock *b = pool; b != NULL; b = b->next) {
    char *start = b->data;
    char *end = b->data + b->pos;
    char *p = (char *) base;
    if (p >= start && p + n == end) {
      b->pos -= n;
      return 1;
    }
  }
  return 0;
}

#include "basic_pool.h"

#include <stdlib.h>
#include <string.h>
#include <stddef.h>

typedef struct PoolBlock {
  char *data;
  size_t size;
  size_t pos;
  struct PoolBlock *next;
} PoolBlock;

typedef struct FreeBlock {
  size_t size;
  struct FreeBlock *next;
} FreeBlock;

static PoolBlock *pool = NULL;
static FreeBlock *free_list = NULL;

static size_t align_up (size_t n) {
  size_t align = _Alignof (max_align_t);
  return (n + align - 1) & ~(align - 1);
}

void basic_pool_init (size_t size) {
  if (pool != NULL) basic_pool_destroy ();
  free_list = NULL;
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
  /* First, try to reuse a freed block. */
  for (FreeBlock **pb = &free_list; *pb != NULL; pb = &(*pb)->next) {
    if ((*pb)->size >= size) {
      FreeBlock *res = *pb;
      *pb = res->next;
      return (char *) res + sizeof (FreeBlock);
    }
  }
  if (pool == NULL) basic_pool_init (1024);
  PoolBlock *b = curr_block ();
  if (b == NULL) return NULL;
  size_t total = sizeof (FreeBlock) + size;
  if (b->pos + total > b->size) {
    size_t newsize = b->size ? b->size * 2 : 1024;
    while (newsize < total) newsize *= 2;
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
  FreeBlock *res = (FreeBlock *) (b->data + b->pos);
  res->size = size;
  res->next = NULL;
  b->pos += total;
  return (char *) res + sizeof (FreeBlock);
}

void basic_pool_reset (void) {
  for (PoolBlock *b = pool; b != NULL; b = b->next) b->pos = 0;
  free_list = NULL;
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
  free_list = NULL;
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

void basic_pool_free (void *p) {
  if (p == NULL) return;
  FreeBlock *b = (FreeBlock *) ((char *) p - sizeof (FreeBlock));
  b->next = free_list;
  free_list = b;
}

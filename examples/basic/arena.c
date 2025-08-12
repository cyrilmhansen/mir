#include "arena.h"
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define ARENA_DEFAULT_CHUNK_SIZE 4096

void arena_init (arena_t *a) { a->head = NULL; }

static arena_chunk_t *new_chunk (size_t size) {
  arena_chunk_t *c = malloc (sizeof (*c) + size);
  if (c == NULL) return NULL;
  c->next = NULL;
  c->used = 0;
  c->size = size;
  return c;
}

void *arena_alloc (arena_t *a, size_t size) {
  size_t align = _Alignof (max_align_t);
  size = (size + align - 1) & ~(align - 1);
  arena_chunk_t *c = a->head;
  if (c == NULL || c->used + size + align > c->size) {
    size_t chunk_size
      = size + align > ARENA_DEFAULT_CHUNK_SIZE ? size + align : ARENA_DEFAULT_CHUNK_SIZE;
    arena_chunk_t *n = new_chunk (chunk_size);
    if (n == NULL) return NULL;
    n->next = c;
    a->head = c = n;
  }
  c->used = (c->used + align - 1) & ~(align - 1);
  void *mem = c->data + c->used;
  c->used += size;
  return mem;
}

char *arena_strdup (arena_t *a, const char *s) {
  size_t len = strlen (s) + 1;
  char *res = arena_alloc (a, len);
  if (res == NULL) return NULL;
  memcpy (res, s, len);
  return res;
}

void arena_reset (arena_t *a) {
  for (arena_chunk_t *c = a->head; c != NULL; c = c->next) c->used = 0;
}

void arena_release (arena_t *a) {
  arena_chunk_t *c = a->head;
  while (c != NULL) {
    arena_chunk_t *next = c->next;
    free (c);
    c = next;
  }
  a->head = NULL;
}

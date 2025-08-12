#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>

typedef struct arena_chunk {
  struct arena_chunk *next;
  size_t used;
  size_t size;
  _Alignas (max_align_t) char data[];
} arena_chunk_t;

typedef struct {
  arena_chunk_t *head;
} arena_t;

void arena_init (arena_t *a);
void *arena_alloc (arena_t *a, size_t size);
char *arena_strdup (arena_t *a, const char *s);
void arena_release (arena_t *a);

#endif /* ARENA_H */

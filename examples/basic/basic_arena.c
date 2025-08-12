#include "basic_arena.h"
#include "arena.h"

static arena_t basic_arena;

void basic_arena_reset (void) {
  for (arena_chunk_t *c = basic_arena.head; c != NULL; c = c->next) c->used = 0;
}

void basic_arena_finish (void) { arena_release (&basic_arena); }

void *basic_arena_alloc (size_t size) { return arena_alloc (&basic_arena, size); }

char *basic_arena_strdup (const char *s) { return arena_strdup (&basic_arena, s); }

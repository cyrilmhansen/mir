#ifndef BASIC_ARENA_H
#define BASIC_ARENA_H

#include <stddef.h>

void basic_arena_reset (void);
void basic_arena_finish (void);
void *basic_arena_alloc (size_t size);
char *basic_arena_strdup (const char *s);

#endif /* BASIC_ARENA_H */

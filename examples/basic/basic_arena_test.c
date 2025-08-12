#include "basic_arena.h"
#include <stdio.h>
#include <string.h>

int main (void) {
  char *s = basic_arena_strdup ("hello");
  if (strcmp (s, "hello") != 0) return 1;
  basic_arena_reset ();
  char *t = basic_arena_strdup ("bye");
  if (strcmp (t, "bye") != 0) return 1;
  basic_arena_finish ();
  printf ("basic_arena_test OK\n");
  return 0;
}

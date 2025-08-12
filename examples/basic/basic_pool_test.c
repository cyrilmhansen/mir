#include "basic_pool.h"
#include <stdio.h>
#include <string.h>

int main (void) {
  basic_pool_init (0);

  char *s = basic_alloc_string (5);
  strcpy (s, "hello");
  if (strcmp (s, "hello") != 0) return 1;

  int *arr = basic_alloc_array (4, sizeof (int), 1);
  for (int i = 0; i < 4; ++i)
    if (arr[i] != 0) return 1;

  basic_pool_reset ();
  char *s2 = basic_alloc_string (3);
  strcpy (s2, "bye");
  if (strcmp (s2, "bye") != 0) return 1;

  basic_pool_destroy ();
  printf ("basic_pool_test OK\n");
  return 0;
}

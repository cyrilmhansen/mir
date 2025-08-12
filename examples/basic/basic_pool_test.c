#include "basic_pool.h"
#include <stdio.h>
#include <string.h>

int main (void) {
  basic_pool_init (0);

  char *s = basic_alloc_string (5);
  strcpy (s, "hello");
  if (strcmp (s, "hello") != 0) return 1;

  int *arr1 = basic_calloc (4, sizeof (int));
  int *arr2 = basic_calloc (4, sizeof (int));
  for (int i = 0; i < 4; ++i)
    if (arr1[i] != 0 || arr2[i] != 0) return 1;
  if (basic_clear_array_pool (arr1, 4, sizeof (int))) return 1;
  if (!basic_clear_array_pool (arr2, 4, sizeof (int))) return 1;

  basic_pool_reset ();
  char *s2 = basic_alloc_string (3);
  strcpy (s2, "bye");
  if (strcmp (s2, "bye") != 0) return 1;

  basic_pool_init (0);
  int *arr3 = basic_calloc (4, sizeof (int));
  for (int i = 0; i < 4; ++i)
    if (arr3[i] != 0) return 1;
  if (basic_clear_array_pool (arr2, 4, sizeof (int))) return 1;
  if (!basic_clear_array_pool (arr3, 4, sizeof (int))) return 1;

  basic_pool_destroy ();
  printf ("basic_pool_test OK\n");
  return 0;
}

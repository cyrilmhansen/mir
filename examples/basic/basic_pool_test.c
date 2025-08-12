#include "basic_pool.h"
#include <stdio.h>
#include <string.h>

int main (void) {
  basic_pool_init (0);

  char *s1 = basic_alloc_string (5);
  strcpy (s1, "hello");
  basic_pool_free (s1);
  char *s2 = basic_alloc_string (5);
  if (s1 != s2) return 1;
  strcpy (s2, "world");
  if (strcmp (s2, "world") != 0) return 1;

  int *arr1 = basic_calloc (4, sizeof (int));
  arr1[0] = 42;
  basic_pool_free (arr1);
  int *arr2 = basic_calloc (4, sizeof (int));
  if (arr2 != arr1) return 1;
  for (int i = 0; i < 4; ++i)
    if (arr2[i] != 0) return 1;

  basic_pool_destroy ();
  printf ("basic_pool_test OK\n");
  return 0;
}

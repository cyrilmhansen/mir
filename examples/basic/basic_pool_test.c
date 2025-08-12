#include "basic_pool.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

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

#if defined(BASIC_USE_LONG_DOUBLE)
  long double *ldarr = basic_alloc_array (4, sizeof (long double), 0);
  for (int i = 0; i < 4; ++i) ldarr[i] = 1.0L;
  if (!basic_clear_array_pool (ldarr, 4, sizeof (long double))) return 1;
  for (int i = 0; i < 4; ++i)
    if (ldarr[i] != 0.0L) return 1;
  if ((uintptr_t) ldarr % _Alignof (long double) != 0) return 1;
#endif

  basic_pool_destroy ();
  printf ("basic_pool_test OK\n");
  return 0;
}

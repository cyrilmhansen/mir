#include "basic_pool.h"
#include "basic_num.h"
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

  basic_num_t *arr1 = basic_calloc (4, sizeof (basic_num_t));
  arr1[0] = BASIC_FROM_INT (42);
  basic_pool_free (arr1);
  basic_num_t *arr2 = basic_calloc (4, sizeof (basic_num_t));
  if (arr2 != arr1) return 1;
  for (int i = 0; i < 4; ++i)
    if (!BASIC_EQ (arr2[i], BASIC_ZERO)) return 1;

  if ((uintptr_t) arr2 % _Alignof (basic_num_t) != 0) return 1;
  for (int i = 0; i < 4; ++i) arr2[i] = BASIC_FROM_INT (i + 1);
  if (!basic_clear_array_pool (arr2, 4, sizeof (basic_num_t))) return 1;
  for (int i = 0; i < 4; ++i)
    if (!BASIC_EQ (arr2[i], BASIC_ZERO)) return 1;

  basic_pool_destroy ();
  printf ("basic_pool_test OK\n");
  return 0;
}

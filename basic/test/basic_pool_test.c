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

  int *arr1 = basic_calloc (4, sizeof (int));
  arr1[0] = 42;
  basic_pool_free (arr1);
  int *arr2 = basic_calloc (4, sizeof (int));
  if (arr2 != arr1) return 1;
  for (int i = 0; i < 4; ++i)
    if (arr2[i] != 0) return 1;

  basic_num_t *narr1 = basic_calloc (4, sizeof (basic_num_t));
  for (int i = 0; i < 4; ++i)
    if (!basic_num_eq (narr1[i], BASIC_ZERO)) return 1;
  if ((uintptr_t) narr1 % _Alignof (basic_num_t) != 0) return 1;
  basic_pool_free (narr1);
  basic_num_t *narr2 = basic_calloc (4, sizeof (basic_num_t));
  if (narr2 != narr1) return 1;
  for (int i = 0; i < 4; ++i)
    if (!basic_num_eq (narr2[i], BASIC_ZERO)) return 1;
  for (int i = 0; i < 4; ++i) narr2[i] = basic_num_from_int (i + 1);
  if (!basic_clear_array_pool (narr2, 4, sizeof (basic_num_t))) return 1;
  for (int i = 0; i < 4; ++i)
    if (!basic_num_eq (narr2[i], BASIC_ZERO)) return 1;

  basic_pool_destroy ();
  printf ("basic_pool_test OK\n");
  return 0;
}

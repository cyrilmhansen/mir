#include "basic_pool.h"
#include "basic_num.h"
#include <stdint.h>
#include <stdio.h>

int main (void) {
  basic_pool_init (0);
  basic_num_init (BASIC_NUM_MODE_FIXED64);
  basic_num_t *arr = basic_calloc (4, sizeof (basic_num_t));
  if (arr == NULL) return 1;
  if ((uintptr_t) arr % _Alignof (basic_num_t) != 0) return 1;
  basic_pool_free (arr);
  basic_pool_destroy ();
  printf ("basic_num_array_align_test OK\n");
  return 0;
}

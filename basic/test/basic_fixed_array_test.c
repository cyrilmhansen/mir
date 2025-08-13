#define BASIC_USE_FIXED64
#include "basic_pool.h"
#include "basic_num.h"
#include <assert.h>
#include <stdint.h>

int main (void) {
  basic_pool_init (0);
  size_t count = 4;
  basic_num_t *arr = basic_alloc_array (count, sizeof (basic_num_t), 1);
  assert (arr != NULL);
  assert ((uintptr_t) arr % _Alignof (basic_num_t) == 0);
  basic_pool_free (arr);
  basic_pool_destroy ();
  return 0;
}

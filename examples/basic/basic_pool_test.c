#include <stdio.h>
#include <string.h>

#include "basic_pool.h"

int main (void) {
  basic_pool_init (1024);

  char *s1 = basic_alloc_string (3);
  memcpy (s1, "abc", 4);
  printf ("s1:%s\n", s1);

  int *arr = basic_alloc_array (4, sizeof (int), 1);
  printf ("arr:%d %d %d %d\n", arr[0], arr[1], arr[2], arr[3]);

  basic_pool_reset ();

  char *s2 = basic_alloc_string (2);
  memcpy (s2, "hi", 3);
  printf ("s2:%s\n", s2);

  basic_pool_destroy ();
  return 0;
}

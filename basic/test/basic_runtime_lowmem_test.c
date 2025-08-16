#include "basic_pool.h"
#include "basic_runtime.h"
#include <stdio.h>
#include <sys/resource.h>
#include <stdint.h>

char *basic_string (int64_t n, const char *s);

/* Test runtime helpers under low-memory conditions. */
int main (void) {
  struct rlimit lim = {32 * 1024 * 1024, 32 * 1024 * 1024};
  setrlimit (RLIMIT_AS, &lim);

  char *s = basic_string (40 * 1024 * 1024, "A");
  if (s != NULL) return 1;

  void *arr = basic_dim_alloc (NULL, basic_num_from_int (40 * 1024 * 1024), BASIC_ZERO);
  if (arr != NULL) return 1;

  printf ("basic_runtime_lowmem_test OK\n");
  return 0;
}

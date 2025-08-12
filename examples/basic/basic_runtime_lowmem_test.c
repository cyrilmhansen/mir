#include "basic_pool.h"
#include "basic_runtime.h"
#include <stdio.h>
#include <sys/resource.h>

char *basic_string (basic_num_t n, const char *s);

/* Test runtime helpers under low-memory conditions. */
int main (void) {
  struct rlimit lim = {32 * 1024 * 1024, 32 * 1024 * 1024};
  setrlimit (RLIMIT_AS, &lim);

  char *s = basic_string (40 * 1024 * 1024, "A");
  if (s != NULL) return 1;

  void *arr = basic_dim_alloc (NULL, (basic_num_t) (40 * 1024 * 1024), 0.0);
  if (arr != NULL) return 1;

  printf ("basic_runtime_lowmem_test OK\n");
  return 0;
}

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "basic_num.h"

void basic_open (basic_num_t n, const char *path);
void basic_close (basic_num_t n);
basic_num_t basic_input_hash (basic_num_t n);

int main (void) {
  basic_num_init (BASIC_NUM_MODE_DOUBLE);
  char path[] = "basic_input_hash_testXXXXXX";
  int fd = mkstemp (path);
  FILE *f = fdopen (fd, "w");
  fputs ("42\n", f);
  fclose (f);

  basic_open (basic_num_from_int (1), path);
  basic_num_t x = basic_input_hash (basic_num_from_int (1));
  basic_close (basic_num_from_int (1));
  unlink (path);
  assert (basic_num_to_int (x) == 42);

  char path2[] = "basic_input_hash_badXXXXXX";
  int fd2 = mkstemp (path2);
  FILE *f2 = fdopen (fd2, "w");
  fputs ("oops\n", f2);
  fclose (f2);

  basic_open (basic_num_from_int (1), path2);
  basic_num_t y = basic_input_hash (basic_num_from_int (1));
  basic_close (basic_num_from_int (1));
  unlink (path2);
  assert (BASIC_EQ (y, BASIC_ZERO));

  return 0;
}

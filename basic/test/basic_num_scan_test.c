#include <assert.h>
#include <stdio.h>

#include "basic_num.h"

int main (void) {
  FILE *in = tmpfile ();
  fputs ("42\n", in);
  rewind (in);
  basic_num_t x;
  assert (basic_num_scan (in, &x));
  assert (basic_num_to_int (x) == 42);
  fclose (in);

  FILE *bad = tmpfile ();
  fputs ("oops\n", bad);
  rewind (bad);
  assert (!basic_num_scan (bad, &x));
  fclose (bad);
  (void) x;
  return 0;
}

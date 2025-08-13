#include <assert.h>
#include <stdio.h>

#include "basic_num.h"

int main (void) {
  FILE *in = tmpfile ();
  fputs ("42\n", in);
  rewind (in);
  basic_num_t x;
  assert (BASIC_NUM_SCAN (in, &x));
  assert (BASIC_TO_INT (x) == 42);
  fclose (in);

  FILE *bad = tmpfile ();
  fputs ("oops\n", bad);
  rewind (bad);
  assert (!BASIC_NUM_SCAN (bad, &x));
  fclose (bad);
  (void) x;
  return 0;
}

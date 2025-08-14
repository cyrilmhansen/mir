#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#ifndef BASIC_USE_FIXED64
#define BASIC_USE_FIXED64
#endif
#include "basic_num.h"

int main (void) {
  basic_num_t two = basic_num_from_int (2);
  basic_num_t three = basic_num_from_int (3);
  basic_num_t five = basic_num_add (two, three);
  assert (basic_num_to_int (five) == 5);
  assert (basic_num_lt (two, three));

  /* test print */
  FILE *tmp = tmpfile ();
  basic_num_print (tmp, five);
  rewind (tmp);
  char buf[16];
  char *ret = fgets (buf, sizeof (buf), tmp);
  (void) ret;
  assert (buf[0] == '5' && buf[1] == '\0');
  fclose (tmp);

  basic_num_t neg_half = basic_num_div (basic_num_from_int (-1), basic_num_from_int (2));
  tmp = tmpfile ();
  basic_num_print (tmp, neg_half);
  rewind (tmp);
  ret = fgets (buf, sizeof (buf), tmp);
  (void) ret;
  assert (strncmp (buf, "-0.5", 4) == 0);
  fclose (tmp);

  /* test scan */
  FILE *in = tmpfile ();
  fputs ("7\n", in);
  rewind (in);
  basic_num_t seven;
  assert (basic_num_scan (in, &seven));
  fclose (in);
  assert (basic_num_to_int (seven) == 7);
  (void) seven;

  in = tmpfile ();
  fputs ("-0.5\n", in);
  rewind (in);
  basic_num_t minus_half;
  assert (basic_num_scan (in, &minus_half));
  fclose (in);
  assert (fabs (fixed64_to_double (minus_half) + 0.5) < 1e-6);
  (void) minus_half;

  FILE *garbage = tmpfile ();
  fputs ("12abc\n", garbage);
  rewind (garbage);
  assert (!basic_num_scan (garbage, &seven));
  fclose (garbage);

  FILE *overflow = tmpfile ();
  fputs ("9223372036854775808\n", overflow);
  rewind (overflow);
  assert (!basic_num_scan (overflow, &seven));
  fclose (overflow);

  return 0;
}

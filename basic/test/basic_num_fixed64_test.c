#include <assert.h>
#include <stdio.h>

#define BASIC_USE_FIXED64
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
  fgets (buf, sizeof (buf), tmp);
  assert (buf[0] == '5' && buf[1] == '\0');
  fclose (tmp);

  /* test scan */
  FILE *in = tmpfile ();
  fputs ("7\n", in);
  rewind (in);
  basic_num_t seven;
  assert (basic_num_scan (in, &seven));
  fclose (in);
  assert (basic_num_to_int (seven) == 7);

  return 0;
}

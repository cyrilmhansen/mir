#include <assert.h>
#include <stdio.h>

#define BASIC_USE_FIXED64
#include "basic_num.h"

int main (void) {
  basic_num_t two = BASIC_FROM_INT (2);
  basic_num_t three = BASIC_FROM_INT (3);
  basic_num_t five = BASIC_ADD (two, three);
  assert (BASIC_TO_INT (five) == 5);
  assert (BASIC_LT (two, three));

  /* test print */
  FILE *tmp = tmpfile ();
  BASIC_NUM_PRINT (tmp, five);
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
  assert (BASIC_NUM_SCAN (in, &seven));
  fclose (in);
  assert (BASIC_TO_INT (seven) == 7);

  return 0;
}

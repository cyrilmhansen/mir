#include <assert.h>
#include <stdio.h>

#include "basic_num_ops.h"

static void run_checks (void) {
  basic_num_t two = basic_num_from_int (2);
  basic_num_t three = basic_num_from_int (3);
  basic_num_t five = basic_num_add (two, three);
  assert (basic_num_to_int (five) == 5);
  assert (basic_num_lt (two, three));
}

int main (void) {
  basic_set_num_mode (BASIC_NUM_MODE_DOUBLE);
  run_checks ();
  basic_set_num_mode (BASIC_NUM_MODE_LONG_DOUBLE);
  run_checks ();
  basic_set_num_mode (BASIC_NUM_MODE_FIXED64);
  run_checks ();
  basic_set_num_mode (BASIC_NUM_MODE_MBF);
  run_checks ();
  return 0;
}

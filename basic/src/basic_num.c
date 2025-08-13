#include "basic_num.h"

static void basic_num_init_double (void);
static void basic_num_init_long_double (void);
static void basic_num_init_fixed64 (void);
static void basic_num_init_mb5 (void);

basic_num_ops_t basic_num;

void basic_num_init (basic_num_mode_t mode) {
  switch (mode) {
  case BASIC_NUM_MODE_LONG_DOUBLE: basic_num_init_long_double (); break;
  case BASIC_NUM_MODE_FIXED64: basic_num_init_fixed64 (); break;
  case BASIC_NUM_MODE_MBASIC5: basic_num_init_mb5 (); break;
  case BASIC_NUM_MODE_DOUBLE:
  default: basic_num_init_double (); break;
  }
}

#include "basic_num.h"

/* Placeholder backend using double implementation for now. */
extern void basic_num_init_double (void);

void basic_num_init_mb5 (void) { basic_num_init_double (); }

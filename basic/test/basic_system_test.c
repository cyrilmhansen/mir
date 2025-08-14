#include <stdio.h>
#include "basic_num.h"

extern basic_num_t basic_system (const char *cmd);
extern char *basic_system_out (void);
extern void basic_free (char *s);

int main (void) {
  basic_num_t r = basic_system ("echo hi");
  printf ("%ld\n", basic_num_to_int (r));
  char *out = basic_system_out ();
  printf ("%s", out);
  basic_free (out);
  return 0;
}

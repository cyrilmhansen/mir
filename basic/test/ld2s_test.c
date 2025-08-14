#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "ryu/ld2s.h"

int main (void) {
  char buf[64];

  ld2s_buffered (0.0L, buf);
  assert (strcmp (buf, "0E0") == 0);

  ld2s_buffered (1.0L, buf);
  assert (strcmp (buf, "1E0") == 0);

  ld2s_buffered (-1.0L, buf);
  assert (strcmp (buf, "-1E0") == 0);

  ld2s_buffered (123.456L, buf);
  assert (strcmp (buf, "1.23456E2") == 0);

  ld2s_buffered (1e30L, buf);
  assert (strcmp (buf, "1E30") == 0);

  ld2s_buffered (-1e-30L, buf);
  assert (strcmp (buf, "-1E-30") == 0);

  char *s = ld2s (1.2345L);
  assert (strcmp (s, "1.2345E0") == 0);
  free (s);

  return 0;
}

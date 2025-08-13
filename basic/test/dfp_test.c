#include "decQuad.h"
#include "decContext.h"
#include <stdio.h>

int main (void) {
  decContext ctx;
  decContextDefault (&ctx, DEC_INIT_DECQUAD);
  decQuad q;
  decQuadFromString (&q, "1.23", &ctx);
  char buf[DECQUAD_String];
  decQuadToString (&q, buf);
  printf ("%s\n", buf);
  return 0;
}

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "basic_num.h"

extern void basic_hcolor (basic_num_t c);
extern uint32_t basic_get_hcolor (void);
extern void basic_set_palette (const uint32_t *colors, size_t n);
extern void basic_set_palette_from_env (void);

int main (void) {
  basic_hcolor (1);
  printf ("%06X\n", basic_get_hcolor ());
  basic_hcolor (0x112233);
  printf ("%06X\n", basic_get_hcolor ());
  uint32_t pal[2] = {0x010203, 0x040506};
  basic_set_palette (pal, 2);
  basic_hcolor (1);
  printf ("%06X\n", basic_get_hcolor ());
  setenv ("BASIC_PALETTE", "A0B0C0,0D0E0F", 1);
  basic_set_palette_from_env ();
  basic_hcolor (0);
  printf ("%06X\n", basic_get_hcolor ());
  basic_hcolor (1);
  printf ("%06X\n", basic_get_hcolor ());
  return 0;
}

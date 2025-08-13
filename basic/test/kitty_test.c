#include "kitty.h"
#include <stdint.h>

int main (void) {
  uint8_t pixel[4] = {255, 0, 0, 255};
  return kitty_draw_image (pixel, 1, 1);
}

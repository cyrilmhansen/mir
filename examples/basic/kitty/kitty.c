#include "kitty.h"
#include "lodepng.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *KITTY_HDR = "\x1b_G";
static const char *KITTY_FTR = "\x1b\\";

static char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static char *base64_encode (const unsigned char *data, size_t input_len, size_t *out_len) {
  *out_len = 4 * ((input_len + 2) / 3);
  char *encoded = malloc (*out_len);
  if (!encoded) return NULL;
  size_t i = 0, j = 0;
  while (i < input_len) {
    uint32_t octet_a = i < input_len ? data[i++] : 0;
    uint32_t octet_b = i < input_len ? data[i++] : 0;
    uint32_t octet_c = i < input_len ? data[i++] : 0;
    uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;
    encoded[j++] = b64_table[(triple >> 18) & 0x3F];
    encoded[j++] = b64_table[(triple >> 12) & 0x3F];
    encoded[j++] = (i > input_len + 1) ? '=' : b64_table[(triple >> 6) & 0x3F];
    encoded[j++] = (i > input_len) ? '=' : b64_table[triple & 0x3F];
  }
  return encoded;
}

int kitty_draw_image (const uint8_t *rgba, uint32_t width, uint32_t height) {
  unsigned char *png = NULL;
  size_t png_size = 0;
  unsigned error = lodepng_encode32 (&png, &png_size, rgba, width, height);
  if (error) return error;
  size_t b64_len = 0;
  char *b64 = base64_encode (png, png_size, &b64_len);
  free (png);
  if (!b64) return 1;

  fwrite (KITTY_HDR, 1, 3, stdout);
  fwrite ("a=T,f=100,t=d,m=1;", 1, strlen ("a=T,f=100,t=d,m=1;"), stdout);
  fwrite (KITTY_FTR, 1, 2, stdout);

  size_t pos = 0;
  const size_t chunk = 4096;
  while (pos < b64_len) {
    size_t n = b64_len - pos;
    if (n > chunk) n = chunk;
    fwrite (KITTY_HDR, 1, 3, stdout);
    fwrite ("m=1;", 1, strlen ("m=1;"), stdout);
    fwrite (b64 + pos, 1, n, stdout);
    fwrite (KITTY_FTR, 1, 2, stdout);
    pos += n;
  }

  fwrite (KITTY_HDR, 1, 3, stdout);
  fwrite ("m=0;", 1, strlen ("m=0;"), stdout);
  fwrite (KITTY_FTR, 1, 2, stdout);
  fflush (stdout);
  free (b64);
  return 0;
}

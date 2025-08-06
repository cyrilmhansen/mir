#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdint.h>

double basic_input (void) {
  double x = 0.0;
  if (scanf ("%lf", &x) != 1) return 0.0;
  return x;
}

void basic_print (double x) { printf ("%g", x); }

void basic_print_str (const char *s) { fputs (s, stdout); }

char *basic_input_str (void) {
  char buf[256];
  if (fgets (buf, sizeof (buf), stdin) == NULL) return strdup ("");
  size_t len = strlen (buf);
  if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
  return strdup (buf);
}

char *basic_get (void) {
  int c = getchar ();
  if (c == EOF) c = 0;
  char *s = malloc (2);
  s[0] = (char) c;
  s[1] = '\0';
  return s;
}

int basic_strcmp (const char *a, const char *b) { return strcmp (a, b); }

typedef struct BasicData {
  int is_str;
  double num;
  char *str;
} BasicData;

BasicData *basic_data_items = NULL;
size_t basic_data_len = 0;
size_t basic_data_pos = 0;

double basic_read (void) {
  if (basic_data_pos >= basic_data_len || basic_data_items[basic_data_pos].is_str) return 0.0;
  return basic_data_items[basic_data_pos++].num;
}

char *basic_read_str (void) {
  if (basic_data_pos >= basic_data_len || !basic_data_items[basic_data_pos].is_str) return "";
  return basic_data_items[basic_data_pos++].str;
}

void basic_restore (void) { basic_data_pos = 0; }

void basic_home (void) { printf ("\x1b[2J\x1b[H"); }

void basic_vtab (double n) { printf ("\x1b[%d;H", (int) n); }

void basic_screen (double m) { (void) m; }

void basic_cls (void) { printf ("\x1b[2J\x1b[H"); }

void basic_color (double c) { printf ("\x1b[%dm", (int) c); }

void basic_key_off (void) {}

void basic_locate (double r, double c) { printf ("\x1b[%d;%dH", (int) r, (int) c); }

void basic_htab (double n) { printf ("\x1b[%dG", (int) n); }
void basic_tab (double n) { basic_htab (n); }

double basic_rnd (double n) {
  static int seeded = 0;
  if (!seeded) {
    srand ((unsigned) time (NULL));
    seeded = 1;
  }
  return ((double) rand () / RAND_MAX) * n;
}

char *basic_chr (double n) {
  char *s = malloc (2);
  s[0] = (char) ((int) n);
  s[1] = '\0';
  return s;
}

char *basic_string (double n, const char *s) {
  int len = (int) n;
  char ch = s != NULL && s[0] != '\0' ? s[0] : '\0';
  char *res = malloc ((size_t) len + 1);
  for (int i = 0; i < len; i++) res[i] = ch;
  res[len] = '\0';
  return res;
}

double basic_int (double x) { return floor (x); }

double basic_timer (void) { return (double) clock () / CLOCKS_PER_SEC; }

char *basic_input_chr (double n) {
  int len = (int) n;
  char *res = malloc ((size_t) len + 1);
  int i = 0;
  for (; i < len; i++) {
    int c = getchar ();
    if (c == EOF) break;
    res[i] = (char) c;
  }
  res[i] = '\0';
  return res;
}

#define BASIC_MEM_SIZE 65536
static uint8_t basic_memory[BASIC_MEM_SIZE];

double basic_peek (double addr) {
  int a = (int) addr;
  if (a < 0 || a >= BASIC_MEM_SIZE) return 0.0;
  return (double) basic_memory[a];
}

void basic_poke (double addr, double value) {
  int a = (int) addr;
  int v = (int) value & 0xff;
  if (a < 0 || a >= BASIC_MEM_SIZE) return;
  basic_memory[a] = (uint8_t) v;
}

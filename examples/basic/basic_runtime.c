#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double basic_input (void) {
  double x = 0.0;
  if (scanf ("%lf", &x) != 1) return 0.0;
  return x;
}

void basic_print (double x) { printf ("%g\n", x); }

void basic_print_str (const char *s) { puts (s); }

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

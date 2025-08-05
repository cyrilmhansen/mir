#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

void basic_home (void) { printf ("\x1b[2J\x1b[H"); }

void basic_vtab (double n) { printf ("\x1b[%d;H", (int) n); }

double basic_rnd (double n) {
  static int seeded = 0;
  if (!seeded) {
    srand ((unsigned) time (NULL));
    seeded = 1;
  }
  return ((double) rand () / RAND_MAX) * n;
}

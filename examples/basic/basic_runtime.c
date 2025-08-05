#include <stdio.h>

double basic_input (void) {
  double x = 0.0;
  if (scanf ("%lf", &x) != 1) return 0.0;
  return x;
}

void basic_print (double x) { printf ("%g\n", x); }

void basic_print_str (const char *s) { puts (s); }

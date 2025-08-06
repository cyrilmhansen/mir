#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdint.h>

static int basic_pos_val = 1;

double basic_input (void) {
  double x = 0.0;
  if (scanf ("%lf", &x) != 1) return 0.0;
  return x;
}

void basic_print (double x) {
  char buf[32];
  int len = snprintf (buf, sizeof (buf), "%g", x);
  basic_pos_val += len;
  fputs (buf, stdout);
}

void basic_print_str (const char *s) {
  for (const char *p = s; *p != '\0'; p++) basic_pos_val = *p == '\n' ? 1 : basic_pos_val + 1;
  fputs (s, stdout);
}

double basic_pos (void) { return (double) basic_pos_val; }

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

#define BASIC_MAX_FILES 16
static FILE *basic_files[BASIC_MAX_FILES];

void basic_open (double n, const char *path) {
  int idx = (int) n;
  if (idx < 0 || idx >= BASIC_MAX_FILES) return;
  if (basic_files[idx] != NULL) {
    fclose (basic_files[idx]);
    basic_files[idx] = NULL;
  }
  FILE *f = fopen (path, "r+");
  if (f == NULL) f = fopen (path, "w+");
  basic_files[idx] = f;
}

void basic_close (double n) {
  int idx = (int) n;
  if (idx < 0 || idx >= BASIC_MAX_FILES) return;
  if (basic_files[idx] != NULL) {
    fclose (basic_files[idx]);
    basic_files[idx] = NULL;
  }
}

void basic_print_hash (double n, double x) {
  int idx = (int) n;
  if (idx < 0 || idx >= BASIC_MAX_FILES || basic_files[idx] == NULL) return;
  fprintf (basic_files[idx], "%g", x);
}

void basic_print_hash_str (double n, const char *s) {
  int idx = (int) n;
  if (idx < 0 || idx >= BASIC_MAX_FILES || basic_files[idx] == NULL) return;
  fputs (s, basic_files[idx]);
}

double basic_input_hash (double n) {
  int idx = (int) n;
  if (idx < 0 || idx >= BASIC_MAX_FILES || basic_files[idx] == NULL) return 0.0;
  double x = 0.0;
  if (fscanf (basic_files[idx], "%lf", &x) != 1) return 0.0;
  return x;
}

char *basic_input_hash_str (double n) {
  int idx = (int) n;
  if (idx < 0 || idx >= BASIC_MAX_FILES || basic_files[idx] == NULL) return strdup ("");
  char buf[256];
  if (fgets (buf, sizeof (buf), basic_files[idx]) == NULL) return strdup ("");
  size_t len = strlen (buf);
  if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
  return strdup (buf);
}

char *basic_get_hash (double n) {
  int idx = (int) n;
  if (idx < 0 || idx >= BASIC_MAX_FILES || basic_files[idx] == NULL) {
    char *s = malloc (2);
    s[0] = 0;
    s[1] = '\0';
    return s;
  }
  int c = fgetc (basic_files[idx]);
  if (c == EOF) c = 0;
  char *s = malloc (2);
  s[0] = (char) c;
  s[1] = '\0';
  return s;
}

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

double basic_abs (double x) { return fabs (x); }

double basic_sgn (double x) { return x > 0 ? 1.0 : x < 0 ? -1.0 : 0.0; }

double basic_sqr (double x) { return sqrt (x); }

double basic_sin (double x) { return sin (x); }

double basic_cos (double x) { return cos (x); }

double basic_tan (double x) { return tan (x); }

double basic_atn (double x) { return atan (x); }

double basic_log (double x) { return log (x); }

double basic_exp (double x) { return exp (x); }

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

char *basic_left (const char *s, double n) {
  size_t len = strlen (s);
  size_t cnt = (size_t) n;
  if (cnt > len) cnt = len;
  char *res = malloc (cnt + 1);
  memcpy (res, s, cnt);
  res[cnt] = '\0';
  return res;
}

char *basic_right (const char *s, double n) {
  size_t len = strlen (s);
  size_t cnt = (size_t) n;
  if (cnt > len) cnt = len;
  return strdup (s + len - cnt);
}

char *basic_mid (const char *s, double start_d, double len_d) {
  size_t len = strlen (s);
  size_t start = (size_t) start_d;
  if (start < 1) start = 1;
  start--;
  if (start >= len) return strdup ("");
  size_t cnt = len_d < 0 ? len - start : (size_t) len_d;
  if (start + cnt > len) cnt = len - start;
  char *res = malloc (cnt + 1);
  memcpy (res, s + start, cnt);
  res[cnt] = '\0';
  return res;
}

double basic_instr (const char *s, const char *sub) {
  if (s == NULL || sub == NULL || *sub == '\0') return 0.0;
  const char *p = strstr (s, sub);
  return p == NULL ? 0.0 : (double) (p - s + 1);
}

double basic_len (const char *s) { return (double) strlen (s); }

double basic_val (const char *s) { return strtod (s, NULL); }

char *basic_str (double n) {
  char buf[32];
  snprintf (buf, sizeof (buf), "%g", n);
  return strdup (buf);
}

double basic_asc (const char *s) {
  return s == NULL || s[0] == '\0' ? 0.0 : (double) (unsigned char) s[0];
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

void basic_text (void) { printf ("\x1b[0m\x1b[2J\x1b[H"); }

void basic_inverse (void) { printf ("\x1b[7m"); }

void basic_normal (void) { printf ("\x1b[0m"); }

void basic_hgr2 (void) { printf ("\x1b[2J\x1b[H"); }

static int current_hcolor = 37;

void basic_hcolor (double c) { current_hcolor = 30 + ((int) c & 7); }

void basic_hplot (double x, double y) {
  printf ("\x1b[%dm\x1b[%d;%dH*\x1b[0m", current_hcolor, (int) y, (int) x);
  fflush (stdout);
}

void basic_stop (void) {
  fflush (stdout);
  exit (0);
}

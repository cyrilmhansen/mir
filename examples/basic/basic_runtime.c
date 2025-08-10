#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdint.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/wait.h>
#include "kitty/kitty.h"
#include "basic_runtime.h"

static int seeded = 0;
static int basic_pos_val = 1;
static int basic_error_handler = 0;
static int basic_line = 0;
static double last_hplot_x = 0.0, last_hplot_y = 0.0;
int basic_line_tracking_enabled = 1;
static char *system_output = NULL;

typedef struct {
  int line;
  long count;
  long long ns;
} LineProfile;

typedef struct {
  const char *name;
  long count;
  long long ns;
} FuncProfile;

typedef struct {
  size_t idx;
  struct timespec start;
} FuncStackEntry;

static LineProfile *line_profiles = NULL;
static size_t line_profiles_len = 0;
static FuncProfile *func_profiles = NULL;
static size_t func_profiles_len = 0;
static FuncStackEntry *func_stack = NULL;
static size_t func_stack_len = 0;
static int last_profile_line = -1;
static struct timespec last_time;

static long long diff_ns (struct timespec a, struct timespec b) {
  return (b.tv_sec - a.tv_sec) * 1000000000LL + (b.tv_nsec - a.tv_nsec);
}

static LineProfile *get_line_profile (int line) {
  for (size_t i = 0; i < line_profiles_len; i++)
    if (line_profiles[i].line == line) return &line_profiles[i];
  line_profiles = realloc (line_profiles, (line_profiles_len + 1) * sizeof (LineProfile));
  line_profiles[line_profiles_len] = (LineProfile) {.line = line, .count = 0, .ns = 0};
  return &line_profiles[line_profiles_len++];
}

static size_t get_func_profile (const char *name) {
  for (size_t i = 0; i < func_profiles_len; i++)
    if (strcmp (func_profiles[i].name, name) == 0) return i;
  func_profiles = realloc (func_profiles, (func_profiles_len + 1) * sizeof (FuncProfile));
  func_profiles[func_profiles_len] = (FuncProfile) {.name = strdup (name), .count = 0, .ns = 0};
  return func_profiles_len++;
}

void basic_profile_reset (void) {
  free (line_profiles);
  if (func_profiles != NULL) {
    for (size_t i = 0; i < func_profiles_len; i++) free ((char *) func_profiles[i].name);
  }
  free (func_profiles);
  free (func_stack);
  line_profiles = NULL;
  func_profiles = NULL;
  func_stack = NULL;
  line_profiles_len = func_profiles_len = func_stack_len = 0;
  last_profile_line = -1;
  clock_gettime (CLOCK_MONOTONIC, &last_time);
}

void basic_profile_line (double line_no) {
  struct timespec now;
  clock_gettime (CLOCK_MONOTONIC, &now);
  if (last_profile_line != -1) {
    LineProfile *lp = get_line_profile (last_profile_line);
    lp->ns += diff_ns (last_time, now);
  }
  LineProfile *curr = get_line_profile ((int) line_no);
  curr->count++;
  last_profile_line = (int) line_no;
  last_time = now;
}

void basic_profile_func_enter (const char *name) {
  struct timespec now;
  clock_gettime (CLOCK_MONOTONIC, &now);
  size_t idx = get_func_profile (name);
  func_profiles[idx].count++;
  func_stack = realloc (func_stack, (func_stack_len + 1) * sizeof (FuncStackEntry));
  func_stack[func_stack_len++] = (FuncStackEntry) {idx, now};
}

void basic_profile_func_exit (const char *name) {
  (void) name;
  if (func_stack_len == 0) return;
  struct timespec now;
  clock_gettime (CLOCK_MONOTONIC, &now);
  FuncStackEntry ent = func_stack[--func_stack_len];
  func_profiles[ent.idx].ns += diff_ns (ent.start, now);
}

static int line_cmp (const void *a, const void *b) {
  const LineProfile *la = a, *lb = b;
  return la->line - lb->line;
}

static int func_cmp (const void *a, const void *b) {
  const FuncProfile *fa = a, *fb = b;
  return strcmp (fa->name, fb->name);
}

void basic_profile_dump (void) {
  struct timespec now;
  clock_gettime (CLOCK_MONOTONIC, &now);
  if (last_profile_line != -1) {
    LineProfile *lp = get_line_profile (last_profile_line);
    lp->ns += diff_ns (last_time, now);
    last_profile_line = -1;
  }
  qsort (line_profiles, line_profiles_len, sizeof (LineProfile), line_cmp);
  qsort (func_profiles, func_profiles_len, sizeof (FuncProfile), func_cmp);
  for (size_t i = 0; i < line_profiles_len; i++) {
    printf ("line %d: count %ld time %.9f\n", line_profiles[i].line, line_profiles[i].count,
            line_profiles[i].ns / 1e9);
  }
  for (size_t i = 0; i < func_profiles_len; i++) {
    printf ("func %s: count %ld time %.9f\n", func_profiles[i].name, func_profiles[i].count,
            func_profiles[i].ns / 1e9);
  }
}

void basic_enable_line_tracking (double on) { basic_line_tracking_enabled = on != 0; }

void basic_set_error_handler (double line) { basic_error_handler = (int) line; }

double basic_get_error_handler (void) { return (double) basic_error_handler; }

void basic_set_line (double line) {
  if (basic_line_tracking_enabled) basic_line = (int) line;
}

double basic_get_line (void) {
  if (!basic_line_tracking_enabled) {
    fprintf (stderr, "line tracking disabled\n");
    exit (1);
  }
  return (double) basic_line;
}

/* Release a string allocated by BASIC runtime helpers. */
void basic_free (char *s) { free (s); }

/* Duplicate a C string using malloc; result must be freed with basic_free. */
char *basic_strdup (const char *s) { return strdup (s); }

double basic_input (void) {
  double x = 0.0;
  if (scanf ("%lf", &x) != 1) return 0.0;
  return x;
}

void basic_print (double x) {
  char buf[32];
  int len = snprintf (buf, sizeof (buf), "%.15g", x);
  basic_pos_val += len;
  fputs (buf, stdout);
}

void basic_print_str (const char *s) {
  for (const char *p = s; *p != '\0'; p++) basic_pos_val = *p == '\n' ? 1 : basic_pos_val + 1;
  fputs (s, stdout);
}

double basic_pos (void) { return (double) basic_pos_val; }

/* Allocate a new string containing input from stdin.
   Caller must free the returned buffer with basic_free. */
char *basic_input_str (void) {
  char buf[256];
  if (fgets (buf, sizeof (buf), stdin) == NULL) return strdup ("");
  size_t len = strlen (buf);
  if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
  return strdup (buf);
}
/* Allocate a one-character string from stdin.
   Caller must free the returned buffer with basic_free. */
char *basic_get (void) {
  int c = getchar ();
  if (c == EOF) c = 0;
  char *s = malloc (2);
  s[0] = (char) c;
  s[1] = '\0';
  return s;
}

/* Non-blocking check for a character from stdin.
   Caller must free the returned buffer with basic_free. */
char *basic_inkey (void) {
  struct timeval tv = {0, 0};
  fd_set fds;
  FD_ZERO (&fds);
  FD_SET (0, &fds);
  if (select (1, &fds, NULL, NULL, &tv) > 0) {
    int c = getchar ();
    if (c == EOF) c = 0;
    char *s = malloc (2);
    s[0] = (char) c;
    s[1] = '\0';
    return s;
  }
  char *s = malloc (2);
  s[0] = 0;
  s[1] = '\0';
  return s;
}

void basic_put (const char *s) {
  int c = s != NULL && s[0] != '\0' ? (unsigned char) s[0] : 0;
  basic_pos_val = c == '\n' ? 1 : basic_pos_val + 1;
  fputc (c, stdout);
}

int basic_strcmp (const char *a, const char *b) {
  if (a == NULL || b == NULL) {
    if (a == b) return 0;
    return a == NULL ? -1 : 1;
  }
  return strcmp (a, b);
}

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
  fprintf (basic_files[idx], "%.15g", x);
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

/* Read a line from an open file and return a newly allocated string.
   Caller must free the result with basic_free. */
char *basic_input_hash_str (double n) {
  int idx = (int) n;
  if (idx < 0 || idx >= BASIC_MAX_FILES || basic_files[idx] == NULL) return strdup ("");
  char buf[256];
  if (fgets (buf, sizeof (buf), basic_files[idx]) == NULL) return strdup ("");
  size_t len = strlen (buf);
  if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = '\0';
  return strdup (buf);
}

/* Read a single character from an open file and return it as a
   newly allocated string. Caller must free the result with basic_free. */
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

void basic_put_hash (double n, const char *s) {
  int idx = (int) n;
  if (idx < 0 || idx >= BASIC_MAX_FILES || basic_files[idx] == NULL) return;
  int c = s != NULL && s[0] != '\0' ? (unsigned char) s[0] : 0;
  fputc (c, basic_files[idx]);
}

double basic_eof (double n) {
  int idx = (int) n;
  if (idx < 0 || idx >= BASIC_MAX_FILES || basic_files[idx] == NULL) return -1.0;
  return feof (basic_files[idx]) ? -1.0 : 0.0;
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

/* Return next DATA string as a newly allocated buffer.
   Caller must free the result with basic_free. */
char *basic_read_str (void) {
  if (basic_data_pos >= basic_data_len || !basic_data_items[basic_data_pos].is_str)
    return basic_strdup ("");
  return basic_strdup (basic_data_items[basic_data_pos++].str);
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

void basic_randomize (double n, double has_seed) {
  if (has_seed != 0.0) {
    srand ((unsigned) n);
  } else {
    srand ((unsigned) time (NULL));
  }
  seeded = 1;
}

double basic_rnd (double n) {
  if (!seeded) {
    srand ((unsigned) time (NULL));
    seeded = 1;
  }
  /* rand () can return RAND_MAX, which would make the result equal to n.
     Scale by RAND_MAX + 1.0 to keep the value in [0, n). */
  return ((double) rand () / ((double) RAND_MAX + 1.0)) * n;
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

/* Allocate a one-character string. Caller must free with basic_free. */
char *basic_chr (double n) {
  char *s = malloc (2);
  s[0] = (char) ((int) n);
  s[1] = '\0';
  return s;
}

/* Return a string of length N filled with the first character of S.
   Caller must free the result with basic_free. */
char *basic_string (double n, const char *s) {
  int len = (int) n;
  char ch = s != NULL && s[0] != '\0' ? s[0] : '\0';
  char *res = malloc ((size_t) len + 1);
  for (int i = 0; i < len; i++) res[i] = ch;
  res[len] = '\0';
  return res;
}

/* Return the leftmost N characters of S as a newly allocated string.
   Caller must free the result with basic_free. */
char *basic_left (const char *s, double n) {
  size_t len = strlen (s);
  size_t cnt = (size_t) n;
  if (cnt > len) cnt = len;
  char *res = malloc (cnt + 1);
  memcpy (res, s, cnt);
  res[cnt] = '\0';
  return res;
}

/* Return the rightmost N characters of S as a newly allocated string.
   Caller must free the result with basic_free. */
char *basic_right (const char *s, double n) {
  size_t len = strlen (s);
  size_t cnt = (size_t) n;
  if (cnt > len) cnt = len;
  return strdup (s + len - cnt);
}

/* Return a substring of S starting at START_D with length LEN_D.
   Caller must free the result with basic_free. */
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

/* Convert a number to a newly allocated string.
   Caller must free the result with basic_free. */
char *basic_str (double n) {
  char buf[32];
  snprintf (buf, sizeof (buf), "%.15g", n);
  return strdup (buf);
}

double basic_asc (const char *s) {
  return s == NULL || s[0] == '\0' ? 0.0 : (double) (unsigned char) s[0];
}

double basic_int (double x) { return floor (x); }

double basic_timer (void) { return (double) clock () / CLOCKS_PER_SEC; }

double basic_time (void) {
  time_t t = time (NULL);
  struct tm *tm_info = localtime (&t);
  return (double) (tm_info->tm_hour * 3600 + tm_info->tm_min * 60 + tm_info->tm_sec);
}

/* Return current time formatted as HH:MM:SS in a newly allocated string.
   Caller must free the result with basic_free. */
char *basic_time_str (void) {
  time_t t = time (NULL);
  struct tm *tm_info = localtime (&t);
  char buf[9];
  strftime (buf, sizeof (buf), "%H:%M:%S", tm_info);
  return strdup (buf);
}

/* Read N characters from stdin and return them as a newly allocated string.
   Caller must free the result with basic_free. */
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

/* 1x1 PNGs for standard 8 terminal colors, base64-encoded.  */
static const char *kitty_color_png[8] = {
  "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAIAAACQd1PeAAAADElEQVR4nGNgYGAAAAAEAAH2FzhVAAAAAElFTkSuQmCC",
  "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAIAAACQd1PeAAAADElEQVR4nGP4z8AAAAMBAQDJ/pLvAAAAAElFTkSuQmCC",
  "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAIAAACQd1PeAAAADElEQVR4nGNg+M8AAAICAQB7CYF4AAAAAElFTkSuQmCC",
  "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAIAAACQd1PeAAAADElEQVR4nGP4/58BAAT/Af9dfQKHAAAAAElFTkSuQmCC",
  "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAIAAACQd1PeAAAADElEQVR4nGNgYPgPAAEDAQAIicLsAAAAAElFTkSuQmCC",
  "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAIAAACQd1PeAAAADElEQVR4nGP4z/AfAAQAAf8iCjrwAAAAAElFTkSuQmCC",
  "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAIAAACQd1PeAAAADElEQVR4nGNg+P8fAAMBAf+2EqLVAAAAAElFTkSuQmCC",
  "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAIAAACQd1PeAAAADElEQVR4nGP4//8/AAX+Av4N70a4AAAAAElFTkSuQmCC",
};

static void basic_kitty_plot (double x, double y) {
  int ix = (int) x, iy = (int) y;
  int color = (current_hcolor - 30) & 7;
  const char *png = kitty_color_png[color];
  printf (
    "\x1b[%d;%dH\x1b]1337;File=inline=1;width=1;height=1;preserveAspectRatio=0:%s\x07\x1b[%d;%dH",
    iy, ix, png, iy, ix + 1);
  fflush (stdout);
}

static void basic_kitty_line (double x0, double y0, double x1, double y1) {
  double dx = x1 - x0, dy = y1 - y0;
  int steps = fabs (dx) > fabs (dy) ? fabs (dx) : fabs (dy);
  double xi = steps ? dx / steps : 0.0;
  double yi = steps ? dy / steps : 0.0;
  for (int i = 0; i <= steps; i++) {
    basic_kitty_plot (x0 + xi * i, y0 + yi * i);
  }
}

void basic_hplot (double x, double y) {
  basic_kitty_plot (x, y);
  last_hplot_x = x;
  last_hplot_y = y;
}

void basic_hplot_to (double x0, double y0, double x1, double y1) {
  basic_kitty_line (x0, y0, x1, y1);
  last_hplot_x = x1;
  last_hplot_y = y1;
}

void basic_hplot_to_current (double x1, double y1) {
  basic_kitty_line (last_hplot_x, last_hplot_y, x1, y1);
  last_hplot_x = x1;
  last_hplot_y = y1;
}

void basic_move (double x, double y) {
  last_hplot_x = x;
  last_hplot_y = y;
}

void basic_draw (double x, double y) {
  basic_kitty_line (last_hplot_x, last_hplot_y, x, y);
  last_hplot_x = x;
  last_hplot_y = y;
}

void basic_draw_line (double x0, double y0, double x1, double y1) {
  basic_kitty_line (x0, y0, x1, y1);
  last_hplot_x = x1;
  last_hplot_y = y1;
}

static void basic_kitty_circle (double x, double y, double r) {
  double prev_x = x + r, prev_y = y;
  int steps = 360;
  for (int i = 1; i <= steps; i++) {
    double ang = 2 * 3.14159265358979323846 * i / steps;
    double nx = x + r * cos (ang);
    double ny = y + r * sin (ang);
    basic_kitty_line (prev_x, prev_y, nx, ny);
    prev_x = nx;
    prev_y = ny;
  }
}

void basic_circle (double x, double y, double r) {
  basic_kitty_circle (x, y, r);
  last_hplot_x = x + r;
  last_hplot_y = y;
}

void basic_rect (double x0, double y0, double x1, double y1) {
  basic_kitty_line (x0, y0, x1, y0);
  basic_kitty_line (x1, y0, x1, y1);
  basic_kitty_line (x1, y1, x0, y1);
  basic_kitty_line (x0, y1, x0, y0);
  last_hplot_x = x0;
  last_hplot_y = y0;
}

void basic_fill (double x0, double y0, double x1, double y1) {
  if (x0 > x1) {
    double t = x0;
    x0 = x1;
    x1 = t;
  }
  if (y0 > y1) {
    double t = y0;
    y0 = y1;
    y1 = t;
  }
  for (int y = (int) y0; y <= (int) y1; y++) {
    basic_kitty_line (x0, y, x1, y);
  }
  last_hplot_x = x1;
  last_hplot_y = y1;
}

void basic_mode (double m) { (void) m; }

void basic_beep (void) {
  fputc ('\a', stdout);
  fflush (stdout);
}

void basic_sound (double f, double d) {
  (void) f;
  (void) d;
  basic_beep ();
}

double basic_system (const char *cmd) {
  free (system_output);
  system_output = NULL;
  FILE *fp = popen (cmd, "r");
  if (fp == NULL) return -1.0;
  size_t len = 0;
  char *out = NULL;
  char buf[256];
  while (fgets (buf, sizeof (buf), fp) != NULL) {
    size_t blen = strlen (buf);
    char *tmp = realloc (out, len + blen + 1);
    if (tmp == NULL) {
      free (out);
      out = NULL;
      len = 0;
      break;
    }
    out = tmp;
    memcpy (out + len, buf, blen);
    len += blen;
    out[len] = '\0';
  }
  int status = pclose (fp);
  system_output = out ? out : strdup ("");
  if (WIFEXITED (status)) return (double) WEXITSTATUS (status);
  return -1.0;
}

char *basic_system_out (void) { return system_output ? strdup (system_output) : strdup (""); }

void basic_stop (void) {
  fflush (stdout);
  exit (0);
}

typedef enum { H_CTX, H_MOD, H_FUNC, H_REG, H_LABEL } HandleKind;

typedef struct {
  HandleKind kind;
  MIR_context_t ctx;
  void *ptr;
} Handle;

static Handle *handle_tab = NULL;
static size_t handle_len = 0;

static double new_handle (HandleKind kind, MIR_context_t ctx, void *ptr) {
  handle_tab = realloc (handle_tab, (handle_len + 1) * sizeof (Handle));
  handle_tab[handle_len] = (Handle) {kind, ctx, ptr};
  return (double) ++handle_len;
}

static Handle *get_handle (double h) {
  size_t idx = (size_t) h;
  if (idx == 0 || (double) idx != h || idx > handle_len) return NULL;
  return &handle_tab[idx - 1];
}

typedef struct {
  MIR_item_t item;
  size_t next_arg;
  size_t nargs;
} FuncHandle;

static double basic_mir_ctx_impl (void) {
  static double ctx_handle = 0;
  if (ctx_handle != 0) return ctx_handle;
  MIR_context_t ctx = MIR_init ();
  ctx_handle = new_handle (H_CTX, ctx, ctx);
  return ctx_handle;
}

double basic_mir_ctx (void) { return basic_mir_ctx_impl (); }

double basic_mir_mod (double ctx_h, const char *name) {
  Handle *h = get_handle (ctx_h);
  if (h == NULL || h->kind != H_CTX) return 0.0;
  MIR_context_t ctx = h->ctx;
  MIR_module_t mod = MIR_new_module (ctx, name);
  return new_handle (H_MOD, ctx, mod);
}

double basic_mir_func (double mod_h, const char *name, double nargs_d) {
  Handle *h = get_handle (mod_h);
  if (h == NULL || h->kind != H_MOD) return 0.0;
  MIR_context_t ctx = h->ctx;
  size_t nargs = (size_t) nargs_d;
  MIR_type_t res = MIR_T_D;
  MIR_var_t *vars = nargs ? malloc (nargs * sizeof (MIR_var_t)) : NULL;
  for (size_t i = 0; i < nargs; i++) {
    char *arg_name = malloc (16);
    sprintf (arg_name, "a%zu", i);
    vars[i].type = MIR_T_D;
    vars[i].name = arg_name;
  }
  MIR_item_t func = MIR_new_func_arr (ctx, name, 1, &res, nargs, vars);
  free (vars); /* arg names are kept by MIR */
  FuncHandle *fh = malloc (sizeof (FuncHandle));
  fh->item = func;
  fh->next_arg = 0;
  fh->nargs = nargs;
  return new_handle (H_FUNC, ctx, fh);
}

double basic_mir_reg (double func_h) {
  Handle *h = get_handle (func_h);
  if (h == NULL || h->kind != H_FUNC) return 0.0;
  FuncHandle *fh = h->ptr;
  MIR_context_t ctx = h->ctx;
  MIR_reg_t r;
  if (fh->next_arg < fh->nargs) {
    char name[16];
    sprintf (name, "a%zu", fh->next_arg++);
    r = MIR_reg (ctx, name, fh->item->u.func);
  } else {
    r = MIR_new_func_reg (ctx, fh->item->u.func, MIR_T_D, NULL);
  }
  return new_handle (H_REG, ctx, (void *) (uintptr_t) r);
}

double basic_mir_label (double func_h) {
  Handle *h = get_handle (func_h);
  if (h == NULL || h->kind != H_FUNC) return 0.0;
  MIR_context_t ctx = h->ctx;
  MIR_label_t lab = MIR_new_label (ctx);
  return new_handle (H_LABEL, ctx, (void *) lab);
}

double basic_mir_emit (double func_h, const char *op, double a, double b, double c) {
  Handle *h = get_handle (func_h);
  if (h == NULL || h->kind != H_FUNC) return 0.0;
  FuncHandle *fh = h->ptr;
  MIR_context_t ctx = h->ctx;
  MIR_insn_code_t code;
  for (code = 0; code < MIR_INSN_BOUND; code++)
    if (strcmp (op, MIR_insn_name (ctx, code)) == 0) break;
  if (code >= MIR_INSN_BOUND) return 0.0;
  double vals[3] = {a, b, c};
  MIR_op_t ops[3];
  size_t nops = 0;
  int out_p;
  while (_MIR_insn_code_op_mode (ctx, code, nops, &out_p) != MIR_OP_BOUND && nops < 3) nops++;
  for (size_t i = 0; i < nops; i++) {
    Handle *oh = get_handle (vals[i]);
    if (oh != NULL && oh->kind == H_REG)
      ops[i] = MIR_new_reg_op (ctx, (MIR_reg_t) (uintptr_t) oh->ptr);
    else if (oh != NULL && oh->kind == H_LABEL)
      ops[i] = MIR_new_label_op (ctx, (MIR_label_t) oh->ptr);
    else
      ops[i] = MIR_new_double_op (ctx, vals[i]);
  }
  MIR_append_insn (ctx, fh->item, MIR_new_insn_arr (ctx, code, nops, ops));
  return 0.0;
}

double basic_mir_emitlbl (double func_h, double lab_h) {
  Handle *fh = get_handle (func_h);
  Handle *lh = get_handle (lab_h);
  if (fh == NULL || lh == NULL || fh->kind != H_FUNC || lh->kind != H_LABEL) return 0.0;
  FuncHandle *f = fh->ptr;
  MIR_append_insn (fh->ctx, f->item, (MIR_insn_t) lh->ptr);
  return 0.0;
}

double basic_mir_ret (double func_h, double reg_h) {
  Handle *fh = get_handle (func_h);
  Handle *rh = get_handle (reg_h);
  if (fh == NULL || rh == NULL || fh->kind != H_FUNC || rh->kind != H_REG) return 0.0;
  FuncHandle *f = fh->ptr;
  MIR_context_t ctx = fh->ctx;
  MIR_reg_t r = (MIR_reg_t) (uintptr_t) rh->ptr;
  MIR_append_insn (ctx, f->item, MIR_new_ret_insn (ctx, 1, MIR_new_reg_op (ctx, r)));
  MIR_finish_func (ctx);
  return 0.0;
}

double basic_mir_finish (double mod_h) {
  Handle *mh = get_handle (mod_h);
  if (mh == NULL || mh->kind != H_MOD) return 0.0;
  MIR_context_t ctx = mh->ctx;
  MIR_module_t mod = mh->ptr;
  MIR_finish_module (ctx);
  MIR_load_module (ctx, mod);
  MIR_link (ctx, MIR_set_interp_interface, NULL);
  return 0.0;
}

double basic_mir_run (double func_h, double a1, double a2, double a3, double a4) {
  Handle *fh = get_handle (func_h);
  if (fh == NULL || fh->kind != H_FUNC) return 0.0;
  FuncHandle *f = fh->ptr;
  MIR_context_t ctx = fh->ctx;
  void *addr = f->item->addr;
  double res = 0.0;
  switch (f->nargs) {
  case 0: res = ((double (*) (void)) addr) (); break;
  case 1: res = ((double (*) (double)) addr) (a1); break;
  case 2: res = ((double (*) (double, double)) addr) (a1, a2); break;
  case 3: res = ((double (*) (double, double, double)) addr) (a1, a2, a3); break;
  case 4: res = ((double (*) (double, double, double, double)) addr) (a1, a2, a3, a4); break;
  default: break;
  }
  return res;
}

double basic_mir_dump (double func_h) {
  Handle *fh = get_handle (func_h);
  if (fh == NULL || fh->kind != H_FUNC) return 0.0;
  FuncHandle *f = fh->ptr;
  MIR_output_item (fh->ctx, stdout, f->item);
  return 0.0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <stdint.h>
#include <sys/select.h>
#include <unistd.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <termios.h>
#endif
#include "basic_num.h"
#include <sys/wait.h>
#include <sys/time.h>
#include "kitty/kitty.h"
#include "basic_runtime.h"
#include "basic_pool.h"
#include "basic_runtime_fixed64.h"
#include "basic_runtime_shared.h"

#if defined(BASIC_USE_LONG_DOUBLE)
#define BASIC_MIR_NUM_T MIR_T_LD
static inline MIR_op_t BASIC_MIR_new_num_op (MIR_context_t ctx, basic_num_t v) {
  return MIR_new_ldouble_op (ctx, v);
}
#define BASIC_MIR_D2I MIR_LD2I
#define BASIC_MIR_I2D MIR_I2LD
#define BASIC_MIR_DMOV MIR_LDMOV
#define BASIC_MIR_DNEG MIR_LDNEG
#define BASIC_MIR_DEQ MIR_LDEQ
#define BASIC_MIR_DNE MIR_LDNE
#define BASIC_MIR_DLT MIR_LDLT
#define BASIC_MIR_DLE MIR_LDLE
#define BASIC_MIR_DGT MIR_LDGT
#define BASIC_MIR_DGE MIR_LDGE
#define BASIC_MIR_DADD MIR_LDADD
#define BASIC_MIR_DSUB MIR_LDSUB
#define BASIC_MIR_DMUL MIR_LDMUL
#define BASIC_MIR_DDIV MIR_LDDIV
#define BASIC_MIR_DBEQ MIR_LDBEQ
#define BASIC_MIR_DBNE MIR_LDBNE
#define BASIC_MIR_DBLT MIR_LDBLT
#define BASIC_MIR_DBLE MIR_LDBLE
#define BASIC_MIR_DBGT MIR_LDBGT
#define BASIC_MIR_DBGE MIR_LDBGE
#else
#ifndef BASIC_MIR_NUM_T
#define BASIC_MIR_NUM_T MIR_T_D
static inline MIR_op_t BASIC_MIR_new_num_op (MIR_context_t ctx, basic_num_t v) {
  return MIR_new_double_op (ctx, v);
}
#define BASIC_MIR_D2I MIR_D2I
#define BASIC_MIR_I2D MIR_I2D
#define BASIC_MIR_DMOV MIR_DMOV
#define BASIC_MIR_DNEG MIR_DNEG
#define BASIC_MIR_DEQ MIR_DEQ
#define BASIC_MIR_DNE MIR_DNE
#define BASIC_MIR_DLT MIR_DLT
#define BASIC_MIR_DLE MIR_DLE
#define BASIC_MIR_DGT MIR_DGT
#define BASIC_MIR_DGE MIR_DGE
#define BASIC_MIR_DADD MIR_DADD
#define BASIC_MIR_DSUB MIR_DSUB
#define BASIC_MIR_DMUL MIR_DMUL
#define BASIC_MIR_DDIV MIR_DDIV
#define BASIC_MIR_DBEQ MIR_DBEQ
#define BASIC_MIR_DBNE MIR_DBNE
#define BASIC_MIR_DBLT MIR_DBLT
#define BASIC_MIR_DBLE MIR_DBLE
#define BASIC_MIR_DBGT MIR_DBGT
#define BASIC_MIR_DBGE MIR_DBGE
#endif
#endif

int basic_pos_val = 1;
static int basic_error_handler = 0;
static int basic_line = 0;
static basic_num_t last_hplot_x = BASIC_ZERO, last_hplot_y = BASIC_ZERO;
static uint32_t basic_palette[8] = {
  0x000000, 0xFF0000, 0x00FF00, 0xFFFF00, 0x0000FF, 0xFF00FF, 0x00FFFF, 0xFFFFFF,
};
static uint32_t current_hcolor = 0xFFFFFF;
static int palette_initialized = 0;
int basic_line_tracking_enabled = 1;
static char *system_output = NULL;
#define BASIC_POOL_SIZE (1024 * 1024)
static void __attribute__ ((constructor)) init_basic_pool (void) {
  basic_pool_init (BASIC_POOL_SIZE);
}
#ifndef _WIN32
static struct termios saved_termios;
static int termios_saved = 0;
static void restore_termios (void) { tcsetattr (STDIN_FILENO, TCSANOW, &saved_termios); }
#endif

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

void basic_profile_line (basic_num_t line_no) {
  struct timespec now;
  clock_gettime (CLOCK_MONOTONIC, &now);
  if (last_profile_line != -1) {
    LineProfile *lp = get_line_profile (last_profile_line);
    lp->ns += diff_ns (last_time, now);
  }
  LineProfile *curr = get_line_profile (basic_num_to_int (line_no));
  curr->count++;
  last_profile_line = basic_num_to_int (line_no);
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

void basic_enable_line_tracking (basic_num_t on) {
  basic_line_tracking_enabled = basic_num_ne (on, BASIC_ZERO);
}

void basic_set_error_handler (basic_num_t line) { basic_error_handler = basic_num_to_int (line); }

basic_num_t basic_get_error_handler (void) { return basic_num_from_int (basic_error_handler); }

void basic_set_line (basic_num_t line) {
  if (basic_line_tracking_enabled) basic_line = basic_num_to_int (line);
}

basic_num_t basic_get_line (void) {
  if (!basic_line_tracking_enabled) {
    fprintf (stderr, "line tracking disabled\n");
    exit (1);
  }
  return basic_num_from_int (basic_line);
}

/* Release a string allocated by BASIC runtime helpers. */
void basic_free (char *s) { basic_pool_free (s); }

/* Duplicate a C string using the BASIC allocator. */
char *basic_strdup (const char *s) {
  size_t len = strlen (s);
  char *res = basic_alloc_string (len);
  if (res != NULL) memcpy (res, s, len);
  return res;
}

static char *next_input_field (void) {
  static char line[256];
  static char *pos = NULL;
  if (pos == NULL || *pos == '\0') {
    if (fgets (line, sizeof (line), stdin) == NULL) {
      line[0] = '\0';
    } else {
      size_t len = strlen (line);
      if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
    }
    pos = line;
  }
  while (*pos == ' ' || *pos == '\t') pos++;
  char *start = pos;
  if (*pos == '"') {
    start = ++pos;
    while (*pos != '\0' && *pos != '"') pos++;
    char *end = pos;
    if (*pos == '"') pos++;
    while (*pos == ' ' || *pos == '\t') pos++;
    if (*pos == ',') {
      pos++;
    } else if (*pos == '\0') {
      pos = NULL;
    }
    size_t len = end - start;
    char *res = basic_alloc_string (len);
    if (res != NULL && len > 0) memcpy (res, start, len);
    return res;
  } else {
    while (*pos != '\0' && *pos != ',') pos++;
    char *end = pos;
    while (end > start && isspace ((unsigned char) end[-1])) end--;
    size_t len = end - start;
    char *res = basic_alloc_string (len);
    if (res != NULL && len > 0) memcpy (res, start, len);
    if (*pos == ',') {
      pos++;
    } else if (*pos == '\0') {
      pos = NULL;
    }
    return res;
  }
}

void basic_print (basic_num_t x) {
  char buf[128];
  int len = basic_num_to_chars (x, buf, sizeof (buf));
  basic_pos_val += len;
  fputs (buf, stdout);
}

void basic_print_str (const char *s) {
  if (!s) s = "";
  for (const char *p = s; *p != '\0'; p++) basic_pos_val = *p == '\n' ? 1 : basic_pos_val + 1;
  fputs (s, stdout);
}

/* Allocate a new string containing input from stdin.
   Caller must free the returned buffer with basic_free. */
char *basic_input_str (void) { return next_input_field (); }
/* Allocate a one-character string from stdin.
   Caller must free the returned buffer with basic_free. */
char *basic_get (void) {
  int c = getchar ();
  if (c == EOF) c = 0;
  char *s = basic_alloc_string (1);
  if (s != NULL) s[0] = (char) c;
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
    char *s = basic_alloc_string (1);
    if (s != NULL) s[0] = (char) c;
    return s;
  }
  char *s = basic_alloc_string (1);
  if (s != NULL) s[0] = 0;
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
FILE *basic_files[BASIC_MAX_FILES];

void basic_open (int64_t n, const char *path) {
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

void basic_close (int64_t n) {
  int idx = (int) n;
  if (idx < 0 || idx >= BASIC_MAX_FILES) return;
  if (basic_files[idx] != NULL) {
    fclose (basic_files[idx]);
    basic_files[idx] = NULL;
  }
}

void basic_print_hash (int64_t n, basic_num_t x) {
  int idx = (int) n;
  if (idx < 0 || idx >= BASIC_MAX_FILES || basic_files[idx] == NULL) return;
  char buf[128];
  basic_num_to_chars (x, buf, sizeof (buf));
  fputs (buf, basic_files[idx]);
}

void basic_print_hash_str (int64_t n, const char *s) {
  int idx = (int) n;
  if (idx < 0 || idx >= BASIC_MAX_FILES || basic_files[idx] == NULL) return;
  fputs (s, basic_files[idx]);
}

/* Read a line from an open file and return a newly allocated string.
   Caller must free the result with basic_free. */
char *basic_input_hash_str (int64_t n) {
  int idx = (int) n;
  if (idx < 0 || idx >= BASIC_MAX_FILES || basic_files[idx] == NULL) {
    char *res = basic_alloc_string (0);
    if (res == NULL) return NULL;
    return res;
  }
  char buf[256];
  if (fgets (buf, sizeof (buf), basic_files[idx]) == NULL) {
    char *res = basic_alloc_string (0);
    if (res == NULL) return NULL;
    return res;
  }
  size_t len = strlen (buf);
  if (len > 0 && buf[len - 1] == '\n') {
    buf[len - 1] = '\0';
    len--;
  }
  char *res = basic_alloc_string (len);
  if (res != NULL) memcpy (res, buf, len);
  return res;
}

/* Read a single character from an open file and return it as a
   newly allocated string. Caller must free the result with basic_free. */
char *basic_get_hash (int64_t n) {
  int idx = (int) n;
  if (idx < 0 || idx >= BASIC_MAX_FILES || basic_files[idx] == NULL) {
    char *s = basic_alloc_string (1);
    if (s != NULL) s[0] = 0;
    return s;
  }
  int c = fgetc (basic_files[idx]);
  if (c == EOF) c = 0;
  char *s = basic_alloc_string (1);
  if (s != NULL) s[0] = (char) c;
  return s;
}

void basic_put_hash (int64_t n, const char *s) {
  int idx = (int) n;
  if (idx < 0 || idx >= BASIC_MAX_FILES || basic_files[idx] == NULL) return;
  int c = s != NULL && s[0] != '\0' ? (unsigned char) s[0] : 0;
  fputc (c, basic_files[idx]);
}

basic_num_t basic_eof (int64_t n) {
  int idx = (int) n;
  if (idx < 0 || idx >= BASIC_MAX_FILES || basic_files[idx] == NULL) return basic_num_from_int (-1);
  return feof (basic_files[idx]) ? basic_num_from_int (-1) : BASIC_ZERO;
}

BasicData *basic_data_items = NULL;
size_t basic_data_len = 0;
size_t basic_data_pos = 0;

/* Return next DATA string as a newly allocated buffer.
   Caller must free the result with basic_free. */
char *basic_read_str (void) {
  if (basic_data_pos >= basic_data_len || !basic_data_items[basic_data_pos].is_str)
    return basic_strdup ("");
  return basic_strdup (basic_data_items[basic_data_pos++].str);
}

void basic_restore (void) { basic_data_pos = 0; }

void *basic_dim_alloc (void *base, basic_num_t len, basic_num_t is_str) {
  /* Arrays are allocated from a simple pool allocator that does not support
     resizing in place.  When re-dimensioning, callers should first release
     any existing storage via basic_clear_array; the old base pointer is
     ignored here and a new block is always allocated. */
  (void) base;
  size_t n = (size_t) basic_num_to_int (len);
  size_t elem_size = basic_num_ne (is_str, BASIC_ZERO) ? sizeof (char *) : sizeof (basic_num_t);
  void *res = basic_alloc_array (n, elem_size, 1);
  if (res == NULL) return NULL;
  return res;
}

void basic_clear_array (void *base, basic_num_t len, basic_num_t is_str) {
  size_t n = (size_t) basic_num_to_int (len);
  int str_p = basic_num_ne (is_str, BASIC_ZERO);
  if (base == NULL || n == 0) return;
  if (str_p) {
    char **sp = (char **) base;
    for (size_t i = 0; i < n; i++) basic_pool_free (sp[i]);
  }
  basic_pool_free (base);
}

void basic_home (void) { printf ("\x1b[2J\x1b[H"); }

void basic_vtab (int64_t n) { printf ("\x1b[%ld;H", (long) n); }

void basic_cls (void) { printf ("\x1b[2J\x1b[H"); }

void basic_color (int64_t c) { printf ("\x1b[%ldm", (long) c); }
void basic_key_off (void) {
#if defined(_WIN32)
  HANDLE h = GetStdHandle (STD_INPUT_HANDLE);
  DWORD mode;
  if (GetConsoleMode (h, &mode))
    SetConsoleMode (h, mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));
#else
  if (!termios_saved) {
    tcgetattr (STDIN_FILENO, &saved_termios);
    atexit (restore_termios);
    termios_saved = 1;
  }
  struct termios t = saved_termios;
  t.c_lflag &= ~(ICANON | ECHO);
  tcsetattr (STDIN_FILENO, TCSANOW, &t);
#endif
}

void basic_locate (int64_t r, int64_t c) { printf ("\x1b[%ld;%ldH", (long) r, (long) c); }

void basic_tab (int64_t n) { printf ("\x1b[%ldG", (long) n); }
void basic_htab (int64_t n) { basic_tab (n); }

basic_num_t basic_abs (basic_num_t x) { return basic_num_fabs (x); }

basic_num_t basic_sgn (basic_num_t x) {
  if (basic_num_gt (x, BASIC_ZERO)) return basic_num_from_int (1);
  if (basic_num_lt (x, BASIC_ZERO)) return basic_num_from_int (-1);
  return BASIC_ZERO;
}

int64_t basic_iabs (int64_t x) { return llabs (x); }

int64_t basic_isgn (int64_t x) {
  if (x > 0) return 1;
  if (x < 0) return -1;
  return 0;
}

basic_num_t basic_sqr (basic_num_t x) { return basic_num_sqrt (x); }

basic_num_t basic_sin (basic_num_t x) { return basic_num_sin (x); }

basic_num_t basic_cos (basic_num_t x) { return basic_num_cos (x); }

basic_num_t basic_tan (basic_num_t x) { return basic_num_tan (x); }

basic_num_t basic_atn (basic_num_t x) { return basic_num_atan (x); }

basic_num_t basic_sinh (basic_num_t x) { return basic_num_sinh (x); }

basic_num_t basic_cosh (basic_num_t x) { return basic_num_cosh (x); }

basic_num_t basic_tanh (basic_num_t x) { return basic_num_tanh (x); }

basic_num_t basic_asinh (basic_num_t x) { return basic_num_asinh (x); }

basic_num_t basic_acosh (basic_num_t x) { return basic_num_acosh (x); }

basic_num_t basic_atanh (basic_num_t x) { return basic_num_atanh (x); }

basic_num_t basic_asin (basic_num_t x) { return basic_num_asin (x); }

basic_num_t basic_acos (basic_num_t x) { return basic_num_acos (x); }

basic_num_t basic_log (basic_num_t x) { return basic_num_log (x); }

basic_num_t basic_log2 (basic_num_t x) { return basic_num_log2 (x); }

basic_num_t basic_log10 (basic_num_t x) { return basic_num_log10 (x); }

basic_num_t basic_exp (basic_num_t x) { return basic_num_exp (x); }

basic_num_t basic_fact (basic_num_t x) {
  if (basic_num_lt (x, BASIC_ZERO)) return BASIC_ZERO;
  long n = basic_num_to_int (x);
  basic_num_t res = basic_num_from_int (1);
  for (long i = 2; i <= n; i++) res = basic_num_mul (res, basic_num_from_int (i));
  return res;
}

basic_num_t basic_pow (basic_num_t x, basic_num_t y) { return basic_num_pow (x, y); }

basic_num_t basic_pi (void) { return basic_num_acos (basic_num_neg (basic_num_from_int (1))); }

/* Allocate a one-character string. Caller must free with basic_free. */
char *basic_chr (int64_t n) {
  char *s = basic_alloc_string (1);
  if (s != NULL) s[0] = (char) n;
  return s;
}

/* Allocate a UTF-8 string for the given Unicode code point.
   Caller must free with basic_free. */
char *basic_unichar (int64_t n) {
  uint32_t code = (uint32_t) n;
  char buf[4];
  size_t len = 0;
  if (code <= 0x7F) {
    buf[0] = (char) code;
    len = 1;
  } else if (code <= 0x7FF) {
    buf[0] = (char) (0xC0 | (code >> 6));
    buf[1] = (char) (0x80 | (code & 0x3F));
    len = 2;
  } else if (code <= 0xFFFF) {
    buf[0] = (char) (0xE0 | (code >> 12));
    buf[1] = (char) (0x80 | ((code >> 6) & 0x3F));
    buf[2] = (char) (0x80 | (code & 0x3F));
    len = 3;
  } else if (code <= 0x10FFFF) {
    buf[0] = (char) (0xF0 | (code >> 18));
    buf[1] = (char) (0x80 | ((code >> 12) & 0x3F));
    buf[2] = (char) (0x80 | ((code >> 6) & 0x3F));
    buf[3] = (char) (0x80 | (code & 0x3F));
    len = 4;
  }
  char *s = basic_alloc_string (len);
  if (s != NULL && len > 0) memcpy (s, buf, len);
  return s;
}

/* Return a string of length N filled with the first character of S.
   Caller must free the result with basic_free. */
char *basic_string (int64_t n, const char *s) {
  int len = (int) n;
  char ch = s != NULL && s[0] != '\0' ? s[0] : '\0';
  char *res = basic_alloc_string ((size_t) len);
  if (res != NULL) {
    for (int i = 0; i < len; i++) res[i] = ch;
  }
  return res;
}

/* Concatenate A and B into a newly allocated string.
   Caller must free the result with basic_free. */
char *basic_concat (const char *a, const char *b) {
  size_t la = a != NULL ? strlen (a) : 0;
  size_t lb = b != NULL ? strlen (b) : 0;
  char *res = basic_alloc_string (la + lb);
  if (res != NULL) {
    memcpy (res, a, la);
    memcpy (res + la, b, lb + 1);
  }
  return res;
}

/* Return a copy of S converted to uppercase as a newly allocated string.
   Caller must free the result with basic_free. */
char *basic_upper (const char *s) {
  size_t len = s != NULL ? strlen (s) : 0;
  char *res = basic_alloc_string (len);
  if (res != NULL)
    for (size_t i = 0; i < len; i++) res[i] = (char) toupper ((unsigned char) s[i]);
  return res;
}

/* Return a copy of S converted to lowercase as a newly allocated string.
   Caller must free the result with basic_free. */
char *basic_lower (const char *s) {
  size_t len = s != NULL ? strlen (s) : 0;
  char *res = basic_alloc_string (len);
  if (res != NULL)
    for (size_t i = 0; i < len; i++) res[i] = (char) tolower ((unsigned char) s[i]);
  return res;
}

/* Return the leftmost N characters of S as a newly allocated string.
   Caller must free the result with basic_free. */
char *basic_left (const char *s, int64_t n) {
  size_t len = s != NULL ? strlen (s) : 0;
  size_t cnt = n > 0 ? (size_t) n : 0;
  if (cnt > len) cnt = len;
  char *res = basic_alloc_string (cnt);
  if (res != NULL) memcpy (res, s, cnt);
  return res;
}

/* Return the rightmost N characters of S as a newly allocated string.
   Caller must free the result with basic_free. */
char *basic_right (const char *s, int64_t n) {
  size_t len = s != NULL ? strlen (s) : 0;
  size_t cnt = n > 0 ? (size_t) n : 0;
  if (cnt > len) cnt = len;
  char *res = basic_alloc_string (cnt);
  if (res != NULL) memcpy (res, s + len - cnt, cnt);
  return res;
}

/* Return a substring of S starting at START with length LEN.
   Caller must free the result with basic_free. */
char *basic_mid (const char *s, int64_t start, int64_t len_d) {
  size_t len = s != NULL ? strlen (s) : 0;
  long start_i = start < 1 ? 1 : start;
  size_t start_pos = (size_t) start_i;
  start_pos--;
  if (start_pos >= len) {
    char *res = basic_alloc_string (0);
    if (res == NULL) return NULL;
    return res;
  }
  size_t cnt = len_d < 0 ? len - start_pos : (size_t) len_d;
  if (start_pos + cnt > len) cnt = len - start_pos;
  char *res = basic_alloc_string (cnt);
  if (res != NULL) memcpy (res, s + start_pos, cnt);
  return res;
}

/* Return a new string with characters of S in reverse order.
   Caller must free the result with basic_free. */
char *basic_mirror (const char *s) {
  size_t len = s != NULL ? strlen (s) : 0;
  char *res = basic_alloc_string (len);
  if (res != NULL) {
    for (size_t i = 0; i < len; i++) res[i] = s[len - 1 - i];
  }
  return res;
}

long basic_instr (const char *s, const char *sub) {
  if (s == NULL || sub == NULL || *sub == '\0') return 0;
  const char *p = strstr (s, sub);
  return p == NULL ? 0 : p - s + 1;
}

long basic_len (const char *s) { return s != NULL ? (long) strlen (s) : 0; }

basic_num_t basic_val (const char *s) {
  if (s == NULL) return BASIC_ZERO;
  return basic_num_from_string (s, NULL);
}

/* Convert a number to a newly allocated string.
   Caller must free the result with basic_free. */
char *basic_str (basic_num_t n) {
  char buf[128];
  basic_num_to_chars (n, buf, sizeof (buf));
  size_t len = strlen (buf);
  char *res = basic_alloc_string (len);
  if (res != NULL) memcpy (res, buf, len);
  return res;
}

long basic_asc (const char *s) { return s == NULL || s[0] == '\0' ? 0 : (unsigned char) s[0]; }

basic_num_t basic_int (basic_num_t x) { return basic_num_floor (x); }

basic_num_t basic_timer (void) {
  return basic_num_div (basic_num_from_int (clock ()), basic_num_from_int (CLOCKS_PER_SEC));
}

basic_num_t basic_time (void) {
  time_t t = time (NULL);
  struct tm *tm_info = localtime (&t);
  return basic_num_from_int (tm_info->tm_hour * 3600 + tm_info->tm_min * 60 + tm_info->tm_sec);
}

/* Return current time formatted as HH:MM:SS in a newly allocated string.
   Caller must free the result with basic_free. */
char *basic_time_str (void) {
  time_t t = time (NULL);
  struct tm *tm_info = localtime (&t);
  char buf[9];
  strftime (buf, sizeof (buf), "%H:%M:%S", tm_info);
  size_t len = strlen (buf);
  char *res = basic_alloc_string (len);
  if (res != NULL) memcpy (res, buf, len);
  return res;
}

basic_num_t basic_date (void) {
  time_t t = time (NULL);
  return basic_num_from_int (t / 86400);
}

char *basic_date_str (void) {
  time_t t = time (NULL);
  struct tm *tm_info = localtime (&t);
  char buf[11];
  strftime (buf, sizeof (buf), "%Y-%m-%d", tm_info);
  size_t len = strlen (buf);
  char *res = basic_alloc_string (len);
  if (res != NULL) memcpy (res, buf, len);
  return res;
}

/* Read N characters from stdin and return them as a newly allocated string.
   Caller must free the result with basic_free. */
char *basic_input_chr (int64_t n) {
  int len = (int) n;
  char *res = basic_alloc_string ((size_t) len);
  int i = 0;
  if (res != NULL) {
    for (; i < len; i++) {
      int c = getchar ();
      if (c == EOF) break;
      res[i] = (char) c;
    }
    res[i] = '\0';
  }
  return res;
}

#define BASIC_MEM_SIZE 65536
static uint8_t basic_memory[BASIC_MEM_SIZE];

basic_num_t basic_peek (basic_num_t addr) {
  int a = basic_num_to_int (addr);
  if (a < 0 || a >= BASIC_MEM_SIZE) return basic_num_from_int (0);
  return basic_num_from_int (basic_memory[a]);
}

void basic_poke (int64_t addr, int64_t value) {
  int a = (int) addr;
  int v = (int) value & 0xff;
  if (a < 0 || a >= BASIC_MEM_SIZE) return;
  basic_memory[a] = (uint8_t) v;
}

void basic_text (void) { printf ("\x1b[0m\x1b[2J\x1b[H"); }

void basic_inverse (void) { printf ("\x1b[7m"); }

void basic_normal (void) { printf ("\x1b[0m"); }

static void basic_ensure_palette (void);

void basic_hgr2 (void) {
  printf ("\x1b[2J\x1b[H");
  basic_ensure_palette ();
  last_hplot_x = BASIC_ZERO;
  last_hplot_y = BASIC_ZERO;
  current_hcolor = basic_palette[7];
}
void basic_set_palette (const uint32_t *colors, size_t n) {
  size_t cnt = n < 8 ? n : 8;
  for (size_t i = 0; i < cnt; i++) basic_palette[i] = colors[i];
}

void basic_set_palette_from_env (void) {
  const char *env = getenv ("BASIC_PALETTE");
  if (env == NULL) return;
  uint32_t colors[8];
  size_t n = 0;
  while (*env && n < 8) {
    char *end;
    unsigned long v = strtoul (env, &end, 16);
    colors[n++] = (uint32_t) (v & 0xFFFFFFu);
    if (*end != ',') break;
    env = end + 1;
  }
  basic_set_palette (colors, n);
}

static void basic_ensure_palette (void) {
  if (!palette_initialized) {
    basic_set_palette_from_env ();
    palette_initialized = 1;
  }
}

void basic_screen (int64_t m) {
#if defined(_WIN32)
  if (m == 0)
    basic_text ();
  else
    basic_hgr2 ();
#else
  if (m != 0) {
    /* Switch to the alternate screen buffer and enter graphics mode. */
    printf ("\x1b[?1049h");
    basic_hgr2 ();
  } else {
    /* Restore the normal screen buffer and return to text mode. */
    printf ("\x1b[?1049l");
    basic_text ();
  }
  fflush (stdout);
#endif
}

#define BASIC_MAX_WIDTH 280
#define BASIC_MAX_HEIGHT 192

void basic_hcolor (basic_num_t c) {
  basic_ensure_palette ();
  int ic = basic_num_to_int (c);
  if (ic >= 0 && ic < 8)
    current_hcolor = basic_palette[ic];
  else
    current_hcolor = (uint32_t) ic & 0xFFFFFFu;
}

uint32_t basic_get_hcolor (void) { return current_hcolor; }

static void basic_kitty_plot (basic_num_t x, basic_num_t y) {
  int ix = basic_num_to_int (x), iy = basic_num_to_int (y);
  if (ix < 0 || ix >= BASIC_MAX_WIDTH || iy < 0 || iy >= BASIC_MAX_HEIGHT) return;
  uint8_t pixel[4]
    = {(current_hcolor >> 16) & 0xFF, (current_hcolor >> 8) & 0xFF, current_hcolor & 0xFF, 0xFF};
  printf ("\x1b[%d;%dH", iy, ix);
  kitty_draw_image (pixel, 1, 1);
  printf ("\x1b[%d;%dH", iy, ix + 1);
  fflush (stdout);
}

static void basic_kitty_line (basic_num_t x0, basic_num_t y0, basic_num_t x1, basic_num_t y1) {
  basic_num_t dx = basic_num_sub (x1, x0), dy = basic_num_sub (y1, y0);
  basic_num_t abs_dx = BASIC_FABS (dx);
  basic_num_t abs_dy = BASIC_FABS (dy);
  basic_num_t max = basic_num_gt (abs_dx, abs_dy) ? abs_dx : abs_dy;
  int steps = basic_num_to_int (max);
  basic_num_t step_num = basic_num_from_int (steps);
  basic_num_t xi = basic_num_eq (step_num, BASIC_ZERO) ? BASIC_ZERO : basic_num_div (dx, step_num);
  basic_num_t yi = basic_num_eq (step_num, BASIC_ZERO) ? BASIC_ZERO : basic_num_div (dy, step_num);

  for (int i = 0; i <= steps; i++) {
    basic_num_t idx = basic_num_from_int (i);
    basic_kitty_plot (basic_num_add (x0, basic_num_mul (xi, idx)),
                      basic_num_add (y0, basic_num_mul (yi, idx)));
  }
}

void basic_hplot (basic_num_t x, basic_num_t y) {
  basic_kitty_plot (x, y);
  last_hplot_x = x;
  last_hplot_y = y;
}

void basic_hplot_to (basic_num_t x0, basic_num_t y0, basic_num_t x1, basic_num_t y1) {
  basic_kitty_line (x0, y0, x1, y1);
  last_hplot_x = x1;
  last_hplot_y = y1;
}

void basic_hplot_to_current (basic_num_t x1, basic_num_t y1) {
  basic_kitty_line (last_hplot_x, last_hplot_y, x1, y1);
  last_hplot_x = x1;
  last_hplot_y = y1;
}

void basic_move (basic_num_t x, basic_num_t y) {
  last_hplot_x = x;
  last_hplot_y = y;
}

void basic_draw (basic_num_t x, basic_num_t y) {
  basic_kitty_line (last_hplot_x, last_hplot_y, x, y);
  last_hplot_x = x;
  last_hplot_y = y;
}

void basic_draw_line (basic_num_t x0, basic_num_t y0, basic_num_t x1, basic_num_t y1) {
  basic_kitty_line (x0, y0, x1, y1);
  last_hplot_x = x1;
  last_hplot_y = y1;
}

static void basic_kitty_circle (basic_num_t x, basic_num_t y, basic_num_t r) {
  basic_num_t prev_x = basic_num_add (x, r), prev_y = y;
  int steps = 360;
  basic_num_t two_pi = basic_num_from_string ("6.28318530717958647692", NULL);
  basic_num_t steps_num = basic_num_from_int (steps);
  for (int i = 1; i <= steps; i++) {
    basic_num_t ang = basic_num_div (basic_num_mul (two_pi, basic_num_from_int (i)), steps_num);
    basic_num_t nx = basic_num_add (x, basic_num_mul (r, basic_num_cos (ang)));
    basic_num_t ny = basic_num_add (y, basic_num_mul (r, basic_num_sin (ang)));
    basic_kitty_line (prev_x, prev_y, nx, ny);
    prev_x = nx;
    prev_y = ny;
  }
}

void basic_circle (basic_num_t x, basic_num_t y, basic_num_t r) {
  basic_kitty_circle (x, y, r);
  last_hplot_x = basic_num_add (x, r);
  last_hplot_y = y;
}

void basic_rect (basic_num_t x0, basic_num_t y0, basic_num_t x1, basic_num_t y1) {
  basic_kitty_line (x0, y0, x1, y0);
  basic_kitty_line (x1, y0, x1, y1);
  basic_kitty_line (x1, y1, x0, y1);
  basic_kitty_line (x0, y1, x0, y0);
  last_hplot_x = x0;
  last_hplot_y = y0;
}

void basic_fill (basic_num_t x0, basic_num_t y0, basic_num_t x1, basic_num_t y1) {
  if (basic_num_gt (x0, x1)) {
    basic_num_t t = x0;
    x0 = x1;
    x1 = t;
  }
  if (basic_num_gt (y0, y1)) {
    basic_num_t t = y0;
    y0 = y1;
    y1 = t;
  }
  int y_start = basic_num_to_int (y0);
  int y_end = basic_num_to_int (y1);
  for (int y = y_start; y <= y_end; y++) {
    basic_num_t yn = basic_num_from_int (y);
    basic_kitty_line (x0, yn, x1, yn);
  }
  last_hplot_x = x1;
  last_hplot_y = y1;
}

void basic_mode (basic_num_t m) { basic_screen (basic_num_to_int (m)); }

void basic_delay (int64_t ms) {
#if defined(_WIN32)
  Sleep ((DWORD) ms);
#else
  useconds_t usec = (useconds_t) (ms * 1000);
  usleep (usec);
#endif
}

void basic_beep (void) {
  fputc ('\a', stdout);
  fflush (stdout);
}

void basic_sound (basic_num_t f, basic_num_t d, basic_num_t v, basic_num_t async) {
  long freq = basic_num_to_int (f);
  long dur = basic_num_to_int (d);
  long vol = basic_num_to_int (v);
  int non_block = basic_num_to_int (async) != 0;
  if (freq <= 0 || dur <= 0) return;
  if (vol < 0) vol = 0;
  if (vol > 100) vol = 100;
#if defined(_WIN32)
  Beep ((DWORD) freq, (DWORD) dur);
  if (!non_block) Sleep ((DWORD) dur);
#else
  const char *term = getenv ("TERM");
  if (term != NULL && strcmp (term, "linux") == 0) {
    fprintf (stdout, "\033[10;%ld]\033[11;%ld]\033[12;%ld]\a", freq, dur, vol);
  } else {
    fputc ('\a', stdout);
  }
  fflush (stdout);
  if (!non_block) usleep ((useconds_t) (dur * 1000));
#endif
}

void basic_sound_off (void) {
#if !defined(_WIN32)
  const char *term = getenv ("TERM");
  if (term != NULL && strcmp (term, "linux") == 0) {
    fputs ("\033[10;0]\033[11;0]", stdout);
    fflush (stdout);
  }
#endif
}

basic_num_t basic_system (const char *cmd) {
  free (system_output);
  system_output = NULL;
  FILE *fp = popen (cmd, "r");
  if (fp == NULL) return basic_num_from_int (-1);
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
  system_output = out ? out : basic_strdup ("");
  if (WIFEXITED (status)) return basic_num_from_int (WEXITSTATUS (status));
  return basic_num_from_int (-1);
}

char *basic_system_out (void) {
  return system_output ? basic_strdup (system_output) : basic_strdup ("");
}

void basic_runtime_fini (void);

void basic_stop (void) {
  fflush (stdout);
  basic_runtime_fini ();
  exit (0);
}

void basic_return_error (void) {
  fflush (stdout);
  if (basic_line_tracking_enabled)
    fprintf (stderr, "RETURN without GOSUB in line %d\n", basic_line);
  else
    fprintf (stderr, "RETURN without GOSUB\n");
  basic_runtime_fini ();
  exit (1);
}

typedef enum { H_CTX, H_MOD, H_FUNC, H_REG, H_LABEL } HandleKind;

typedef struct {
  HandleKind kind;
  MIR_context_t ctx;
  void *ptr;
} Handle;

static Handle *handle_tab = NULL;
static size_t handle_len = 0;

void basic_runtime_fini (void) {
  if (func_profiles != NULL) {
    for (size_t i = 0; i < func_profiles_len; i++) free ((char *) func_profiles[i].name);
  }
  free (line_profiles);
  free (func_profiles);
  free (func_stack);
  line_profiles = NULL;
  func_profiles = NULL;
  func_stack = NULL;
  line_profiles_len = func_profiles_len = func_stack_len = 0;
  last_profile_line = -1;
  for (size_t i = 0; i < handle_len; ++i) {
    if (handle_tab[i].kind == H_CTX) {
      MIR_finish (handle_tab[i].ctx);
    }
  }
  free (handle_tab);
  handle_tab = NULL;
  handle_len = 0;
  basic_pool_destroy ();
}

static basic_num_t new_handle (HandleKind kind, MIR_context_t ctx, void *ptr) {
  handle_tab = realloc (handle_tab, (handle_len + 1) * sizeof (Handle));
  handle_tab[handle_len] = (Handle) {kind, ctx, ptr};
  return basic_num_from_int (++handle_len);
}

static Handle *get_handle (basic_num_t h) {
  size_t idx = (size_t) basic_num_to_int (h);
  if (idx == 0 || basic_num_ne (basic_num_from_int (idx), h) || idx > handle_len) return NULL;
  return &handle_tab[idx - 1];
}

typedef struct {
  MIR_item_t item;
  size_t next_arg;
  size_t nargs;
} FuncHandle;

static basic_num_t basic_mir_ctx_impl (void) {
  static basic_num_t ctx_handle = BASIC_ZERO;
  if (basic_num_ne (ctx_handle, BASIC_ZERO)) return ctx_handle;
  MIR_context_t ctx = MIR_init ();
  ctx_handle = new_handle (H_CTX, ctx, ctx);
  return ctx_handle;
}

basic_num_t basic_mir_ctx (void) { return basic_mir_ctx_impl (); }

basic_num_t basic_mir_mod (basic_num_t ctx_h, const char *name) {
  Handle *h = get_handle (ctx_h);
  if (h == NULL || h->kind != H_CTX) return BASIC_ZERO;
  MIR_context_t ctx = h->ctx;
  MIR_module_t mod = MIR_new_module (ctx, name);
  basic_runtime_fixed64_init (ctx);
  return new_handle (H_MOD, ctx, mod);
}

basic_num_t basic_mir_func (basic_num_t mod_h, const char *name, basic_num_t nargs_d) {
  Handle *h = get_handle (mod_h);
  if (h == NULL || h->kind != H_MOD) return BASIC_ZERO;
  MIR_context_t ctx = h->ctx;
  size_t nargs = (size_t) basic_num_to_int (nargs_d);
  MIR_type_t res = BASIC_MIR_NUM_T;
  MIR_var_t *vars = nargs ? basic_pool_alloc (nargs * sizeof (MIR_var_t)) : NULL;
  for (size_t i = 0; i < nargs; i++) {
    size_t len = snprintf (NULL, 0, "a%zu", i);
    char *arg_name = basic_alloc_string (len);
    snprintf (arg_name, len + 1, "a%zu", i);
    vars[i].type = BASIC_MIR_NUM_T;
    vars[i].name = arg_name;
  }
  MIR_item_t func = MIR_new_func_arr (ctx, name, 1, &res, nargs, vars);
  FuncHandle *fh = basic_pool_alloc (sizeof (FuncHandle));
  fh->item = func;
  fh->next_arg = 0;
  fh->nargs = nargs;
  return new_handle (H_FUNC, ctx, fh);
}

basic_num_t basic_mir_reg (basic_num_t func_h) {
  Handle *h = get_handle (func_h);
  if (h == NULL || h->kind != H_FUNC) return BASIC_ZERO;
  FuncHandle *fh = h->ptr;
  MIR_context_t ctx = h->ctx;
  MIR_reg_t r;
  if (fh->next_arg < fh->nargs) {
    size_t len = snprintf (NULL, 0, "a%zu", fh->next_arg);
    char *name = basic_alloc_string (len);
    snprintf (name, len + 1, "a%zu", fh->next_arg++);
    r = MIR_reg (ctx, name, fh->item->u.func);
  } else {
    r = MIR_new_func_reg (ctx, fh->item->u.func, BASIC_MIR_NUM_T, NULL);
  }
  return new_handle (H_REG, ctx, (void *) (uintptr_t) r);
}

basic_num_t basic_mir_label (basic_num_t func_h) {
  Handle *h = get_handle (func_h);
  if (h == NULL || h->kind != H_FUNC) return BASIC_ZERO;
  MIR_context_t ctx = h->ctx;
  MIR_label_t lab = MIR_new_label (ctx);
  return new_handle (H_LABEL, ctx, (void *) lab);
}

basic_num_t basic_mir_emit (basic_num_t func_h, const char *op, basic_num_t a, basic_num_t b,
                            basic_num_t c) {
  Handle *h = get_handle (func_h);
  if (h == NULL || h->kind != H_FUNC) return BASIC_ZERO;
  FuncHandle *fh = h->ptr;
  MIR_context_t ctx = h->ctx;
#if defined(BASIC_USE_LONG_DOUBLE)
  char op_buf[16];
  if (op != NULL && op[0] == 'D') {
    snprintf (op_buf, sizeof (op_buf), "LD%s", op + 1);
    op = op_buf;
  }
#endif
  MIR_insn_code_t code;
  for (code = 0; code < MIR_INSN_BOUND; code++)
    if (strcasecmp (op, MIR_insn_name (ctx, code)) == 0) break;
  if (code >= MIR_INSN_BOUND) return BASIC_ZERO;
  basic_num_t vals[3] = {a, b, c};
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
      ops[i] = BASIC_MIR_new_num_op (ctx, vals[i]);
  }
  if (basic_mir_emit_fixed64 (ctx, fh->item, code, ops, nops)) return BASIC_ZERO;
  MIR_append_insn (ctx, fh->item, MIR_new_insn_arr (ctx, code, nops, ops));
  return BASIC_ZERO;
}

basic_num_t basic_mir_emitlbl (basic_num_t func_h, basic_num_t lab_h) {
  Handle *fh = get_handle (func_h);
  Handle *lh = get_handle (lab_h);
  if (fh == NULL || lh == NULL || fh->kind != H_FUNC || lh->kind != H_LABEL) return BASIC_ZERO;
  FuncHandle *f = fh->ptr;
  MIR_append_insn (fh->ctx, f->item, (MIR_insn_t) lh->ptr);
  return BASIC_ZERO;
}

basic_num_t basic_mir_ret (basic_num_t func_h, basic_num_t reg_h) {
  Handle *fh = get_handle (func_h);
  Handle *rh = get_handle (reg_h);
  if (fh == NULL || rh == NULL || fh->kind != H_FUNC || rh->kind != H_REG) return BASIC_ZERO;
  FuncHandle *f = fh->ptr;
  MIR_context_t ctx = fh->ctx;
  MIR_reg_t r = (MIR_reg_t) (uintptr_t) rh->ptr;
  MIR_append_insn (ctx, f->item, MIR_new_ret_insn (ctx, 1, MIR_new_reg_op (ctx, r)));
  MIR_finish_func (ctx);
  return BASIC_ZERO;
}

basic_num_t basic_mir_finish (basic_num_t mod_h) {
  Handle *mh = get_handle (mod_h);
  if (mh == NULL || mh->kind != H_MOD) return BASIC_ZERO;
  MIR_context_t ctx = mh->ctx;
  MIR_module_t mod = mh->ptr;
  MIR_finish_module (ctx);
  MIR_load_module (ctx, mod);
  MIR_link (ctx, MIR_set_interp_interface, NULL);
  return BASIC_ZERO;
}

basic_num_t basic_mir_run (basic_num_t func_h, basic_num_t a1, basic_num_t a2, basic_num_t a3,
                           basic_num_t a4) {
  Handle *fh = get_handle (func_h);
  if (fh == NULL || fh->kind != H_FUNC) return BASIC_ZERO;
  FuncHandle *f = fh->ptr;
  MIR_context_t ctx = fh->ctx;
  (void) ctx;
  void *addr = f->item->addr;
  basic_num_t res = BASIC_ZERO;
  switch (f->nargs) {
  case 0: res = ((basic_num_t (*) (void)) addr) (); break;
  case 1: res = ((basic_num_t (*) (basic_num_t)) addr) (a1); break;
  case 2: res = ((basic_num_t (*) (basic_num_t, basic_num_t)) addr) (a1, a2); break;
  case 3:
    res = ((basic_num_t (*) (basic_num_t, basic_num_t, basic_num_t)) addr) (a1, a2, a3);
    break;
  case 4:
    res = ((basic_num_t (*) (basic_num_t, basic_num_t, basic_num_t, basic_num_t)) addr) (a1, a2, a3,
                                                                                         a4);
    break;
  default: break;
  }
  return res;
}

basic_num_t basic_mir_dump (basic_num_t func_h) {
  Handle *fh = get_handle (func_h);
  if (fh == NULL || fh->kind != H_FUNC) return BASIC_ZERO;
  FuncHandle *f = fh->ptr;
  MIR_output_item (fh->ctx, stdout, f->item);
  return BASIC_ZERO;
}

void __attribute__ ((weak)) basic_eval (const char *cmd) { (void) cmd; }

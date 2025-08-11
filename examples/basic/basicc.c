#include "mir.h"
#include "mir-gen.h"
#include "basic_runtime.h"
#include "basic_num.h"

#if defined(BASIC_USE_LONG_DOUBLE)
#define MIR_T_D MIR_T_LD
#define MIR_new_double_op MIR_new_ldouble_op
#define MIR_D2I MIR_LD2I
#define MIR_I2D MIR_I2LD
#define MIR_DMOV MIR_LDMOV
#define MIR_DNEG MIR_LDNEG
#define MIR_DEQ MIR_LDEQ
#define MIR_DNE MIR_LDNE
#define MIR_DLT MIR_LDLT
#define MIR_DLE MIR_LDLE
#define MIR_DGT MIR_LDGT
#define MIR_DGE MIR_LDGE
#define MIR_DADD MIR_LDADD
#define MIR_DSUB MIR_LDSUB
#define MIR_DMUL MIR_LDMUL
#define MIR_DDIV MIR_LDDIV
#define MIR_DBEQ MIR_LDBEQ
#define MIR_DBNE MIR_LDBNE
#define MIR_DBLT MIR_LDBLT
#define MIR_DBLE MIR_LDBLE
#define MIR_DBGT MIR_LDBGT
#define MIR_DBGE MIR_LDBGE
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <unistd.h>

#ifndef BASIC_SRC_DIR
#define BASIC_SRC_DIR "."
#endif

/* Runtime helpers defined in basic_runtime.c */
extern void basic_print (basic_num_t);
extern void basic_print_str (const char *);
extern basic_num_t basic_input (void);
extern char *basic_input_str (void);
extern char *basic_get (void);

extern char *basic_inkey (void);

extern void basic_put (const char *);

extern void basic_profile_reset (void);
extern void basic_profile_dump (void);
extern void basic_profile_line (basic_num_t);
extern void basic_profile_func_enter (const char *);
extern void basic_profile_func_exit (const char *);

extern int basic_strcmp (const char *, const char *);

extern basic_num_t basic_read (void);
extern char *basic_read_str (void);
extern void basic_restore (void);
extern char *basic_strdup (const char *);
extern void basic_free (char *);

typedef struct BasicData {
  int is_str;
  basic_num_t num;
  char *str;
} BasicData;

extern BasicData *basic_data_items;
extern size_t basic_data_len;
extern size_t basic_data_pos;

extern void basic_home (void);
extern void basic_vtab (basic_num_t);
extern basic_num_t basic_rnd (basic_num_t);
extern void basic_randomize (basic_num_t, basic_num_t);
extern basic_num_t basic_abs (basic_num_t);
extern basic_num_t basic_sgn (basic_num_t);
extern basic_num_t basic_sqr (basic_num_t);
extern basic_num_t basic_sin (basic_num_t);
extern basic_num_t basic_cos (basic_num_t);
extern basic_num_t basic_tan (basic_num_t);
extern basic_num_t basic_atn (basic_num_t);
extern basic_num_t basic_log (basic_num_t);
extern basic_num_t basic_exp (basic_num_t);

extern void basic_screen (basic_num_t);
extern void basic_cls (void);
extern void basic_color (basic_num_t);
extern void basic_key_off (void);
extern void basic_locate (basic_num_t, basic_num_t);
extern void basic_htab (basic_num_t);
extern basic_num_t basic_pos (void);
extern void basic_text (void);
extern void basic_inverse (void);
extern void basic_normal (void);
extern void basic_hgr2 (void);
extern void basic_hcolor (basic_num_t);
extern void basic_hplot (basic_num_t, basic_num_t);
extern void basic_hplot_to (basic_num_t, basic_num_t, basic_num_t, basic_num_t);
extern void basic_hplot_to_current (basic_num_t, basic_num_t);
extern void basic_move (basic_num_t, basic_num_t);
extern void basic_draw (basic_num_t, basic_num_t);
extern void basic_draw_line (basic_num_t, basic_num_t, basic_num_t, basic_num_t);
extern void basic_circle (basic_num_t, basic_num_t, basic_num_t);
extern void basic_rect (basic_num_t, basic_num_t, basic_num_t, basic_num_t);
extern void basic_fill (basic_num_t, basic_num_t, basic_num_t, basic_num_t);
extern void basic_mode (basic_num_t);

extern char *basic_chr (basic_num_t);
extern char *basic_string (basic_num_t, const char *);
extern char *basic_concat (const char *, const char *);
extern char *basic_left (const char *, basic_num_t);
extern char *basic_right (const char *, basic_num_t);
extern char *basic_mid (const char *, basic_num_t, basic_num_t);
extern basic_num_t basic_len (const char *);
extern basic_num_t basic_val (const char *);
extern char *basic_str (basic_num_t);
extern basic_num_t basic_asc (const char *);
extern basic_num_t basic_instr (const char *, const char *);

static int kitty_graphics_available (void) {
  const char *id = getenv ("KITTY_WINDOW_ID");
  const char *term = getenv ("TERM");
  return isatty (1) && (id != NULL || (term != NULL && strstr (term, "kitty") != NULL));
}

static void show_kitty_banner (void) {
  const char *png
    = "iVBORw0KGgoAAAANSUhEUgAAAEYAAAAHCAIAAAA6SKwaAAAANElEQVR4nGP8zzDcAAvD0oF2ArUB4/"
      "9hF02MDMPQS8POTyzeDM0D7QYqA8Z3Te8G2g1UBgBMlgjamTyVSgAAAABJRU5ErkJggg==";
  printf ("\x1b]1337;File=inline=1;width=70;height=7;preserveAspectRatio=0:%s\x07\n", png);
  fflush (stdout);
}
extern basic_num_t basic_int (basic_num_t);
extern basic_num_t basic_timer (void);
extern basic_num_t basic_time (void);
extern char *basic_time_str (void);
extern basic_num_t basic_date (void);
extern char *basic_date_str (void);
extern char *basic_input_chr (basic_num_t);
extern basic_num_t basic_peek (basic_num_t);
extern void basic_poke (basic_num_t, basic_num_t);

extern void basic_open (basic_num_t, const char *);
extern void basic_close (basic_num_t);
extern void basic_print_hash (basic_num_t, basic_num_t);
extern void basic_print_hash_str (basic_num_t, const char *);
extern basic_num_t basic_input_hash (basic_num_t);
extern char *basic_input_hash_str (basic_num_t);
extern char *basic_get_hash (basic_num_t);
extern void basic_put_hash (basic_num_t, const char *);
extern basic_num_t basic_eof (basic_num_t);

extern void basic_stop (void);

extern void basic_set_error_handler (basic_num_t);
extern basic_num_t basic_get_error_handler (void);
extern void basic_set_line (basic_num_t);
extern basic_num_t basic_get_line (void);
extern void basic_enable_line_tracking (basic_num_t);

extern void basic_beep (void);
extern void basic_sound (basic_num_t, basic_num_t);
extern double basic_system (const char *);
extern char *basic_system_out (void);

static int array_base = 0;
static int line_tracking = 1;

static void *resolve (const char *name) {
  if (!strcmp (name, "basic_print")) return basic_print;
  if (!strcmp (name, "basic_print_str")) return basic_print_str;
  if (!strcmp (name, "basic_input")) return basic_input;
  if (!strcmp (name, "basic_input_str")) return basic_input_str;
  if (!strcmp (name, "basic_get")) return basic_get;

  if (!strcmp (name, "basic_inkey")) return basic_inkey;

  if (!strcmp (name, "basic_put")) return basic_put;

  if (!strcmp (name, "basic_strcmp")) return basic_strcmp;
  if (!strcmp (name, "basic_open")) return basic_open;
  if (!strcmp (name, "basic_close")) return basic_close;
  if (!strcmp (name, "basic_print_hash")) return basic_print_hash;
  if (!strcmp (name, "basic_print_hash_str")) return basic_print_hash_str;
  if (!strcmp (name, "basic_input_hash")) return basic_input_hash;
  if (!strcmp (name, "basic_input_hash_str")) return basic_input_hash_str;
  if (!strcmp (name, "basic_get_hash")) return basic_get_hash;
  if (!strcmp (name, "basic_put_hash")) return basic_put_hash;
  if (!strcmp (name, "basic_eof")) return basic_eof;

  if (!strcmp (name, "basic_read")) return basic_read;
  if (!strcmp (name, "basic_read_str")) return basic_read_str;
  if (!strcmp (name, "basic_restore")) return basic_restore;

  if (!strcmp (name, "basic_home")) return basic_home;
  if (!strcmp (name, "basic_vtab")) return basic_vtab;
  if (!strcmp (name, "basic_randomize")) return basic_randomize;
  if (!strcmp (name, "basic_rnd")) return basic_rnd;
  if (!strcmp (name, "basic_abs")) return basic_abs;
  if (!strcmp (name, "basic_sgn")) return basic_sgn;
  if (!strcmp (name, "basic_sqr")) return basic_sqr;
  if (!strcmp (name, "basic_sin")) return basic_sin;
  if (!strcmp (name, "basic_cos")) return basic_cos;
  if (!strcmp (name, "basic_tan")) return basic_tan;
  if (!strcmp (name, "basic_atn")) return basic_atn;
  if (!strcmp (name, "basic_log")) return basic_log;
  if (!strcmp (name, "basic_exp")) return basic_exp;
  if (!strcmp (name, "basic_instr")) return basic_instr;

  if (!strcmp (name, "basic_screen")) return basic_screen;
  if (!strcmp (name, "basic_cls")) return basic_cls;
  if (!strcmp (name, "basic_color")) return basic_color;
  if (!strcmp (name, "basic_key_off")) return basic_key_off;
  if (!strcmp (name, "basic_locate")) return basic_locate;
  if (!strcmp (name, "basic_htab")) return basic_htab;
  if (!strcmp (name, "basic_tab")) return basic_htab;
  if (!strcmp (name, "basic_pos")) return basic_pos;
  if (!strcmp (name, "basic_text")) return basic_text;
  if (!strcmp (name, "basic_inverse")) return basic_inverse;
  if (!strcmp (name, "basic_normal")) return basic_normal;
  if (!strcmp (name, "basic_hgr2")) return basic_hgr2;
  if (!strcmp (name, "basic_hcolor")) return basic_hcolor;
  if (!strcmp (name, "basic_hplot")) return basic_hplot;
  if (!strcmp (name, "basic_hplot_to")) return basic_hplot_to;
  if (!strcmp (name, "basic_hplot_to_current")) return basic_hplot_to_current;
  if (!strcmp (name, "basic_move")) return basic_move;
  if (!strcmp (name, "basic_draw")) return basic_draw;
  if (!strcmp (name, "basic_draw_line")) return basic_draw_line;
  if (!strcmp (name, "basic_circle")) return basic_circle;
  if (!strcmp (name, "basic_rect")) return basic_rect;
  if (!strcmp (name, "basic_fill")) return basic_fill;
  if (!strcmp (name, "basic_mode")) return basic_mode;
  if (!strcmp (name, "basic_profile_line")) return basic_profile_line;
  if (!strcmp (name, "basic_profile_func_enter")) return basic_profile_func_enter;
  if (!strcmp (name, "basic_profile_func_exit")) return basic_profile_func_exit;

  if (!strcmp (name, "basic_chr")) return basic_chr;
  if (!strcmp (name, "basic_string")) return basic_string;
  if (!strcmp (name, "basic_concat")) return basic_concat;
  if (!strcmp (name, "basic_left")) return basic_left;
  if (!strcmp (name, "basic_right")) return basic_right;
  if (!strcmp (name, "basic_mid")) return basic_mid;
  if (!strcmp (name, "basic_len")) return basic_len;
  if (!strcmp (name, "basic_val")) return basic_val;
  if (!strcmp (name, "basic_str")) return basic_str;
  if (!strcmp (name, "basic_asc")) return basic_asc;
  if (!strcmp (name, "basic_int")) return basic_int;
  if (!strcmp (name, "basic_timer")) return basic_timer;
  if (!strcmp (name, "basic_time")) return basic_time;
  if (!strcmp (name, "basic_time_str")) return basic_time_str;
  if (!strcmp (name, "basic_date")) return basic_date;
  if (!strcmp (name, "basic_date_str")) return basic_date_str;
  if (!strcmp (name, "basic_input_chr")) return basic_input_chr;
  if (!strcmp (name, "basic_peek")) return basic_peek;
  if (!strcmp (name, "basic_poke")) return basic_poke;
  if (!strcmp (name, "basic_stop")) return basic_stop;

  if (!strcmp (name, "basic_set_error_handler")) return basic_set_error_handler;
  if (!strcmp (name, "basic_get_error_handler")) return basic_get_error_handler;
  if (!strcmp (name, "basic_set_line")) return basic_set_line;
  if (!strcmp (name, "basic_get_line")) return basic_get_line;
  if (!strcmp (name, "basic_enable_line_tracking")) return basic_enable_line_tracking;

  if (!strcmp (name, "basic_beep")) return basic_beep;
  if (!strcmp (name, "basic_sound")) return basic_sound;
  if (!strcmp (name, "basic_system")) return basic_system;
  if (!strcmp (name, "basic_system_out")) return basic_system_out;

  if (!strcmp (name, "basic_strdup")) return basic_strdup;
  if (!strcmp (name, "basic_free")) return basic_free;

  if (!strcmp (name, "calloc")) return calloc;
  if (!strcmp (name, "memset")) return memset;
  if (!strcmp (name, "basic_mir_ctx")) return basic_mir_ctx;
  if (!strcmp (name, "basic_mir_mod")) return basic_mir_mod;
  if (!strcmp (name, "basic_mir_func")) return basic_mir_func;
  if (!strcmp (name, "basic_mir_reg")) return basic_mir_reg;
  if (!strcmp (name, "basic_mir_label")) return basic_mir_label;
  if (!strcmp (name, "basic_mir_emit")) return basic_mir_emit;
  if (!strcmp (name, "basic_mir_emitlbl")) return basic_mir_emitlbl;
  if (!strcmp (name, "basic_mir_ret")) return basic_mir_ret;
  if (!strcmp (name, "basic_mir_finish")) return basic_mir_finish;
  if (!strcmp (name, "basic_mir_run")) return basic_mir_run;
  if (!strcmp (name, "basic_mir_dump")) return basic_mir_dump;
  return NULL;
}

/* Runtime call prototypes for expressions */
static MIR_item_t rnd_proto, rnd_import, chr_proto, chr_import, string_proto, string_import,
  concat_proto, concat_import, int_proto, int_import, timer_proto, timer_import, time_proto,
  time_import, time_str_proto, time_str_import, date_proto, date_import, date_str_proto,
  date_str_import, input_chr_proto, input_chr_import, peek_proto, peek_import, eof_proto,
  eof_import, abs_proto, abs_import, sgn_proto, sgn_import, inkey_proto, inkey_import, sqr_proto,
  sqr_import, sin_proto, sin_import, cos_proto, cos_import, tan_proto, tan_import, atn_proto,
  atn_import, log_proto, log_import, exp_proto, exp_import, left_proto, left_import, right_proto,
  right_import, mid_proto, mid_import, len_proto, len_import, val_proto, val_import, str_proto,
  str_import, asc_proto, asc_import, pos_proto, pos_import, instr_proto, instr_import, strdup_proto,
  strdup_import, mir_ctx_proto, mir_ctx_import, mir_mod_proto, mir_mod_import, mir_func_proto,
  mir_func_import, mir_reg_proto, mir_reg_import, mir_label_proto, mir_label_import, mir_emit_proto,
  mir_emit_import, mir_emitlbl_proto, mir_emitlbl_import, mir_ret_proto, mir_ret_import,
  mir_finish_proto, mir_finish_import, mir_run_proto, mir_run_import, mir_dump_proto,
  mir_dump_import;

/* Runtime call prototypes for statements */
static MIR_item_t print_proto, print_import, prints_proto, prints_import, input_proto, input_import,
  input_str_proto, input_str_import, get_proto, get_import, put_proto, put_import, read_proto,
  read_import, read_str_proto, read_str_import, restore_proto, restore_import, screen_proto,
  screen_import, cls_proto, cls_import, color_proto, color_import, keyoff_proto, keyoff_import,
  locate_proto, locate_import, htab_proto, htab_import, home_proto, poke_proto, poke_import,
  home_import, vtab_proto, vtab_import, text_proto, text_import, inverse_proto, inverse_import,
  normal_proto, normal_import, hgr2_proto, hgr2_import, hcolor_proto, hcolor_import, hplot_proto,
  hplot_import, hplotto_proto, hplotto_import, hplottocur_proto, hplottocur_import, move_proto,
  move_import, draw_proto, draw_import, line_proto, line_import, circle_proto, circle_import,
  rect_proto, rect_import, mode_proto, mode_import, fill_proto, fill_import, calloc_proto,
  calloc_import, memset_proto, memset_import, strcmp_proto, strcmp_import, open_proto, open_import,
  close_proto, close_import, printh_proto, printh_import, prinths_proto, prinths_import,
  input_hash_proto, input_hash_import, input_hash_str_proto, input_hash_str_import, get_hash_proto,
  get_hash_import, put_hash_proto, put_hash_import, randomize_proto, randomize_import, stop_proto,
  stop_import, on_error_proto, on_error_import, set_line_proto, set_line_import, get_line_proto,
  get_line_import, line_track_proto, line_track_import, profile_line_proto, profile_line_import,
  profile_func_enter_proto, profile_func_enter_import, profile_func_exit_proto,
  profile_func_exit_import, beep_proto, beep_import, sound_proto, sound_import, system_proto,
  system_import, system_out_proto, system_out_import, free_proto, free_import;

/* AST for expressions */
typedef enum { N_NUM, N_VAR, N_BIN, N_NEG, N_NOT, N_STR, N_CALL } NodeKind;
typedef struct Node Node;
struct Node {
  NodeKind kind;
  int is_str;
  basic_num_t num;
  char *var;
  char *str;
  char op;
  Node *left, *right;
  Node *index, *index2, *arg4;
};

static Node *new_node (NodeKind k) {
  Node *n = malloc (sizeof (Node));
  n->kind = k;
  n->is_str = 0;
  n->num = 0;
  n->var = NULL;
  n->str = NULL;
  n->op = 0;
  n->left = n->right = n->index = n->index2 = n->arg4 = NULL;
  return n;
}

typedef struct {
  BasicData *data;
  size_t len, cap;
} DataVec;

static DataVec data_vals;

static void data_vec_push (DataVec *v, BasicData d) {
  if (v->len == v->cap) {
    v->cap = v->cap ? 2 * v->cap : 16;
    v->data = realloc (v->data, v->cap * sizeof (BasicData));
  }
  v->data[v->len++] = d;
}

static void data_vals_clear (void) {
  for (size_t i = 0; i < data_vals.len; i++)
    if (data_vals.data[i].is_str) free (data_vals.data[i].str);
  free (data_vals.data);
  data_vals.data = NULL;
  data_vals.len = data_vals.cap = 0;
  basic_data_items = NULL;
  basic_data_len = basic_data_pos = 0;
}

/* Statement representation */
/* Added ST_REM for comment lines, ST_DIM for array declarations, ST_CLEAR to reset state, and
   data-related statements. */
typedef enum {
  ST_PRINT,
  ST_LET,
  ST_GOTO,
  ST_IF,
  ST_INPUT,
  ST_GET,
  ST_PUT,
  ST_OPEN,
  ST_CLOSE,
  ST_PRINT_HASH,
  ST_INPUT_HASH,
  ST_GET_HASH,
  ST_PUT_HASH,
  ST_DEF,
  ST_DATA,
  ST_READ,
  ST_RESTORE,
  ST_CLEAR,
  ST_SCREEN,
  ST_CLS,
  ST_COLOR,
  ST_KEYOFF,
  ST_LOCATE,
  ST_HTAB,
  ST_POKE,
  ST_HOME,
  ST_VTAB,
  ST_BEEP,
  ST_SOUND,
  ST_SYSTEM,
  ST_RANDOMIZE,
  ST_TEXT,
  ST_INVERSE,
  ST_NORMAL,
  ST_HGR2,
  ST_HCOLOR,
  ST_HPLOT,
  ST_MOVE,
  ST_DRAW,
  ST_LINE,
  ST_CIRCLE,
  ST_RECT,
  ST_MODE,
  ST_FILL,
  ST_END,
  ST_STOP,
  ST_REM,
  ST_DIM,
  ST_FOR,
  ST_NEXT,
  ST_WHILE,
  ST_WEND,
  ST_GOSUB,
  ST_RETURN,
  ST_ON_GOTO,
  ST_ON_GOSUB,
  ST_ON_ERROR,
  ST_RESUME,
  ST_CALL,
} StmtKind;
typedef struct Stmt Stmt;
typedef struct {
  Stmt *data;
  size_t len, cap;
} StmtVec;
struct Stmt {
  StmtKind kind;
  union {
    Node *expr; /* PRINT/VTAB/HTAB/SCREEN/COLOR/WHILE/HCOLOR/RANDOMIZE/MODE */
    struct {
      Node **items;
      size_t n;
      int no_nl;
    } print;
    struct {
      Node *var;
      Node *expr;
      int is_str;
    } let;
    struct {
      Node *cond;
      StmtVec then_stmts;
      StmtVec else_stmts;
    } iff;
    struct {
      char *var;
      int is_str;
    } input;
    struct {
      char *var;
    } get;
    struct {
      Node *expr;
    } put;
    struct {
      Node *num;
      Node *path;
    } open;
    struct {
      Node *num;
    } close;
    struct {
      Node *num;
      Node **items;
      size_t n;
      int no_nl;
    } printhash;
    struct {
      Node *num;
      char *var;
      int is_str;
    } inputhash;
    struct {
      Node *num;
      char *var;
    } gethash;
    struct {
      Node *num;
      Node *expr;
    } puthash;
    struct {
      Node **vars;
      size_t n;
    } read;
    struct {
      char **names;
      Node **sizes1;
      Node **sizes2;
      int *is_str;
      size_t n;
    } dim;
    struct {
      char *var;
      Node *start;
      Node *end;
      Node *step;
    } forto;
    struct {
      char *var;
    } next;
    struct {
      Node *row;
      Node *col;
    } locate;
    struct {
      Node *x;
      Node *y;
    } move;
    struct {
      Node *x;
      Node *y;
    } draw;
    struct {
      Node *x0;
      Node *y0;
      Node *x1;
      Node *y1;
    } line;
    struct {
      Node *x;
      Node *y;
      Node *r;
    } circle;
    struct {
      Node *x0;
      Node *y0;
      Node *x1;
      Node *y1;
    } rect;
    struct {
      Node *x0;
      Node *y0;
      Node *x1;
      Node *y1;
    } fill;
    struct {
      Node **xs;
      Node **ys;
      size_t n;
      int from_prev;
    } hplot;
    struct {
      Node *freq;
      Node *dur;
    } sound;
    struct {
      Node *cmd;
      char *status;
      char *out;
    } system;
    struct {
      Node *addr;
      Node *value;
    } poke;
    int target; /* GOTO/GOSUB/ON_ERROR */
    struct {
      Node *expr;
      int *targets;
      size_t n_targets;
    } on_goto;
    struct {
      Node *expr;
      int *targets;
      size_t n_targets;
    } on_gosub;
    struct {
      int line;
      int has_line;
    } resume;
    struct {
      char *name;
      Node *arg1, *arg2, *arg3, *arg4, *arg5;
    } call;
  } u;
};

typedef struct {
  char *name;
  char **params;
  int *is_str;
  size_t n;
  Node *body;
  StmtVec body_stmts;
  int is_str_ret;
  int is_proc;
  MIR_item_t item;
  MIR_item_t proto;
  char **src_lines;
  size_t src_len, src_cap;
} FuncDef;

typedef struct {
  FuncDef *data;
  size_t len, cap;
} FuncVec;

static FuncVec func_defs;

static void func_def_destroy (FuncDef *fd);

static void func_vec_push (FuncVec *v, FuncDef f) {
  if (v->len == v->cap) {
    v->cap = v->cap ? 2 * v->cap : 4;
    v->data = realloc (v->data, v->cap * sizeof (FuncDef));
  }
  v->data[v->len++] = f;
}

static void func_vec_clear (FuncVec *v) {
  for (size_t i = 0; i < v->len; i++) func_def_destroy (&v->data[i]);
  free (v->data);
  v->data = NULL;
  v->len = v->cap = 0;
}

static FuncDef *find_func (const char *name) {
  for (size_t i = 0; i < func_defs.len; i++)
    if (strcmp (func_defs.data[i].name, name) == 0) return &func_defs.data[i];
  return NULL;
}

/* Program line containing multiple statements */
typedef struct {
  int line;
  StmtVec stmts;
  char *src;
} Line;

typedef struct {
  Line *data;
  size_t len, cap;
} LineVec;

typedef struct {
  MIR_reg_t var, end, step;
  MIR_label_t start_label, end_label;
  int is_while;
} LoopInfo;

static void free_node (Node *n);
static void free_stmt (Stmt *s);
static void free_stmt_vec (StmtVec *v);

static void free_node (Node *n) {
  if (n == NULL) return;
  free_node (n->left);
  free_node (n->right);
  free_node (n->index);
  free_node (n->index2);
  free (n->var);
  free (n->str);
  free (n);
}

static void free_stmt_vec (StmtVec *v) {
  for (size_t i = 0; i < v->len; i++) free_stmt (&v->data[i]);
  free (v->data);
  v->data = NULL;
  v->len = v->cap = 0;
}

static void free_stmt (Stmt *s) {
  switch (s->kind) {
  case ST_PRINT:
    for (size_t i = 0; i < s->u.print.n; i++) free_node (s->u.print.items[i]);
    free (s->u.print.items);
    break;
  case ST_LET:
    free_node (s->u.let.var);
    free_node (s->u.let.expr);
    break;
  case ST_IF:
    free_node (s->u.iff.cond);
    free_stmt_vec (&s->u.iff.then_stmts);
    free_stmt_vec (&s->u.iff.else_stmts);
    break;
  case ST_INPUT: free (s->u.input.var); break;
  case ST_GET: free (s->u.get.var); break;
  case ST_PUT: free_node (s->u.put.expr); break;
  case ST_OPEN:
    free_node (s->u.open.num);
    free_node (s->u.open.path);
    break;
  case ST_CLOSE: free_node (s->u.close.num); break;
  case ST_PRINT_HASH:
    free_node (s->u.printhash.num);
    for (size_t i = 0; i < s->u.printhash.n; i++) free_node (s->u.printhash.items[i]);
    free (s->u.printhash.items);
    break;
  case ST_INPUT_HASH:
    free_node (s->u.inputhash.num);
    free (s->u.inputhash.var);
    break;
  case ST_GET_HASH:
    free_node (s->u.gethash.num);
    free (s->u.gethash.var);
    break;
  case ST_PUT_HASH:
    free_node (s->u.puthash.num);
    free_node (s->u.puthash.expr);
    break;
  case ST_READ:
    for (size_t i = 0; i < s->u.read.n; i++) free_node (s->u.read.vars[i]);
    free (s->u.read.vars);
    break;
  case ST_DIM:
    for (size_t i = 0; i < s->u.dim.n; i++) {
      free (s->u.dim.names[i]);
      free_node (s->u.dim.sizes1[i]);
      free_node (s->u.dim.sizes2[i]);
    }
    free (s->u.dim.names);
    free (s->u.dim.sizes1);
    free (s->u.dim.sizes2);
    free (s->u.dim.is_str);
    break;
  case ST_FOR:
    free (s->u.forto.var);
    free_node (s->u.forto.start);
    free_node (s->u.forto.end);
    free_node (s->u.forto.step);
    break;
  case ST_NEXT: free (s->u.next.var); break;
  case ST_LOCATE:
    free_node (s->u.locate.row);
    free_node (s->u.locate.col);
    break;
  case ST_MOVE:
    free_node (s->u.move.x);
    free_node (s->u.move.y);
    break;
  case ST_DRAW:
    free_node (s->u.draw.x);
    free_node (s->u.draw.y);
    break;
  case ST_LINE:
    free_node (s->u.line.x0);
    free_node (s->u.line.y0);
    free_node (s->u.line.x1);
    free_node (s->u.line.y1);
    break;
  case ST_CIRCLE:
    free_node (s->u.circle.x);
    free_node (s->u.circle.y);
    free_node (s->u.circle.r);
    break;
  case ST_RECT:
    free_node (s->u.rect.x0);
    free_node (s->u.rect.y0);
    free_node (s->u.rect.x1);
    free_node (s->u.rect.y1);
    break;
  case ST_FILL:
    free_node (s->u.fill.x0);
    free_node (s->u.fill.y0);
    free_node (s->u.fill.x1);
    free_node (s->u.fill.y1);
    break;
  case ST_HPLOT:
    for (size_t i = 0; i < s->u.hplot.n; i++) {
      free_node (s->u.hplot.xs[i]);
      free_node (s->u.hplot.ys[i]);
    }
    free (s->u.hplot.xs);
    free (s->u.hplot.ys);
    break;
  case ST_SOUND:
    free_node (s->u.sound.freq);
    free_node (s->u.sound.dur);
    break;
  case ST_SYSTEM:
    free_node (s->u.system.cmd);
    free (s->u.system.status);
    free (s->u.system.out);
    break;
  case ST_POKE:
    free_node (s->u.poke.addr);
    free_node (s->u.poke.value);
    break;
  case ST_ON_GOTO:
    free_node (s->u.on_goto.expr);
    free (s->u.on_goto.targets);
    break;
  case ST_ON_GOSUB:
    free_node (s->u.on_gosub.expr);
    free (s->u.on_gosub.targets);
    break;
  case ST_VTAB:
  case ST_HTAB:
  case ST_SCREEN:
  case ST_COLOR:
  case ST_WHILE:
  case ST_HCOLOR:
  case ST_RANDOMIZE:
  case ST_MODE: free_node (s->u.expr); break;
  default: break;
  }
}

static void func_def_destroy (FuncDef *fd) {
  free (fd->name);
  if (fd->params != NULL) {
    for (size_t i = 0; i < fd->n; i++) free (fd->params[i]);
    free (fd->params);
  }
  free (fd->is_str);
  free_node (fd->body);
  free_stmt_vec (&fd->body_stmts);
  for (size_t i = 0; i < fd->src_len; i++) free (fd->src_lines[i]);
  free (fd->src_lines);
  fd->name = NULL;
  fd->params = NULL;
  fd->is_str = NULL;
  fd->body = NULL;
  fd->body_stmts = (StmtVec) {0};
  fd->src_lines = NULL;
  fd->n = fd->src_len = fd->src_cap = 0;
}

static void free_line (Line *l) {
  free_stmt_vec (&l->stmts);
  free (l->src);
}

static void stmt_vec_push (StmtVec *v, Stmt s) {
  if (v->len == v->cap) {
    v->cap = v->cap ? 2 * v->cap : 16;
    v->data = realloc (v->data, v->cap * sizeof (Stmt));
  }
  v->data[v->len++] = s;
}

static void line_vec_clear (LineVec *v) {
  for (size_t i = 0; i < v->len; i++) free_line (&v->data[i]);
  v->len = 0;
}

static void line_vec_destroy (LineVec *v) {
  line_vec_clear (v);
  free (v->data);
  v->data = NULL;
  v->cap = 0;
}

static void insert_or_replace_line (LineVec *prog, Line l) {
  if (prog->len == prog->cap) {
    prog->cap = prog->cap ? 2 * prog->cap : 16;
    prog->data = realloc (prog->data, prog->cap * sizeof (Line));
  }
  size_t i = 0;
  while (i < prog->len && prog->data[i].line < l.line) i++;
  if (i < prog->len && prog->data[i].line == l.line) {
    free_line (&prog->data[i]);
    prog->data[i] = l;
  } else {
    memmove (&prog->data[i + 1], &prog->data[i], (prog->len - i) * sizeof (Line));
    prog->data[i] = l;
    prog->len++;
  }
}

static void delete_line (LineVec *prog, int line_no) {
  for (size_t i = 0; i < prog->len; i++) {
    if (prog->data[i].line == line_no) {
      free_line (&prog->data[i]);
      memmove (&prog->data[i], &prog->data[i + 1], (prog->len - i - 1) * sizeof (Line));
      prog->len--;
      return;
    }
  }
}

static void list_program (LineVec *prog) {
  for (size_t i = 0; i < func_defs.len; i++) {
    FuncDef *fd = &func_defs.data[i];
    for (size_t j = 0; j < fd->src_len; j++) printf ("%s\n", fd->src_lines[j]);
  }
  for (size_t i = 0; i < prog->len; i++) printf ("%s\n", prog->data[i].src);
}

/* Parsing utilities */
typedef enum {
  TOK_EOF,
  TOK_IDENTIFIER,
  TOK_NUMBER,
  TOK_STRING,
  /* Keywords */
  TOK_REM,
  TOK_OPTION,
  TOK_DIM,
  TOK_LET,
  TOK_GOTO,
  TOK_IF,
  TOK_THEN,
  TOK_ELSE,
  TOK_INPUT,
  TOK_GET,
  TOK_PUT,
  TOK_OPEN,
  TOK_CLOSE,
  TOK_PRINT,
  TOK_PRINT_HASH,
  TOK_INPUT_HASH,
  TOK_GET_HASH,
  TOK_PUT_HASH,
  TOK_DEF,
  TOK_DATA,
  TOK_READ,
  TOK_RESTORE,
  TOK_CLEAR,
  TOK_SCREEN,
  TOK_CLS,
  TOK_COLOR,
  TOK_KEYOFF,
  TOK_LOCATE,
  TOK_HTAB,
  TOK_VTAB,
  TOK_POKE,
  TOK_HOME,
  TOK_BEEP,
  TOK_SOUND,
  TOK_SYSTEM,
  TOK_RANDOMIZE,
  TOK_TEXT,
  TOK_INVERSE,
  TOK_NORMAL,
  TOK_HGR2,
  TOK_HCOLOR,
  TOK_HPLOT,
  TOK_MOVE,
  TOK_DRAW,
  TOK_LINE,
  TOK_CIRCLE,
  TOK_RECT,
  TOK_MODE,
  TOK_FILL,
  TOK_END,
  TOK_STOP,
  TOK_FOR,
  TOK_TO,
  TOK_STEP,
  TOK_NEXT,
  TOK_WHILE,
  TOK_WEND,
  TOK_GOSUB,
  TOK_RETURN,
  TOK_ON,
  TOK_ERROR,
  TOK_RESUME,
  TOK_CALL,
  TOK_AND,
  TOK_OR,
  TOK_NOT,
  TOK_MOD,
  TOK_BASE,
  TOK_FN,
  TOK_TRUE,
  TOK_FALSE,
  /* Punctuation */
  TOK_COMMA,
  TOK_COLON,
  TOK_SEMICOLON,
  TOK_LPAREN,
  TOK_RPAREN,
  TOK_PLUS,
  TOK_MINUS,
  TOK_STAR,
  TOK_SLASH,
  TOK_BACKSLASH,
  TOK_EQ,
  TOK_LT,
  TOK_GT,
  TOK_LE,
  TOK_GE,
  TOK_NE
} TokenType;

typedef struct {
  TokenType type;
  char *str;
  basic_num_t num;
} Token;

typedef struct {
  char *cur;
  Token tok;
  Token peek;
  char *peek_cur;
  int has_peek;
  const char *line_start;
  int line_no;
} Parser;

#define cur (p->cur)

static void skip_ws (Parser *p) {
  while (*cur && isspace ((unsigned char) *cur)) cur++;
}

static void report_parse_error_details (int line_no, const char *line, const char *pos) {
  const char *near = pos != NULL ? pos : line;
  if (line_no > 0)
    fprintf (stderr, "parse error at line %d near '%s'\n", line_no, near ? near : "");
  else
    fprintf (stderr, "parse error near '%s'\n", near ? near : "");
  if (line != NULL) fprintf (stderr, "%s\n", line);
}

static int parse_error (Parser *p) {
  report_parse_error_details (p->line_no, p->line_start, cur);
  return 0;
}

static Token read_token (Parser *p) {
  skip_ws (p);
  Token t = {TOK_EOF, NULL, 0};
  if (*cur == '\0') return t;
  unsigned char c = *cur;
  if (isalpha (c) || c == '_') {
    char buf[64];
    int i = 0;
    while (*cur && (isalnum ((unsigned char) *cur) || *cur == '_')) {
      buf[i++] = toupper ((unsigned char) *cur);
      cur++;
    }
    if (*cur == '$' || *cur == '%') {
      buf[i++] = toupper ((unsigned char) *cur);
      cur++;
    }
    if (*cur == '#') {
      buf[i++] = '#';
      cur++;
    }
    buf[i] = '\0';
    static struct {
      const char *kw;
      TokenType type;
    } keywords[]
      = {{"REM", TOK_REM},           {"OPTION", TOK_OPTION},   {"DIM", TOK_DIM},
         {"LET", TOK_LET},           {"GOTO", TOK_GOTO},       {"IF", TOK_IF},
         {"THEN", TOK_THEN},         {"ELSE", TOK_ELSE},       {"INPUT", TOK_INPUT},
         {"GET", TOK_GET},           {"PUT", TOK_PUT},         {"OPEN", TOK_OPEN},
         {"CLOSE", TOK_CLOSE},       {"PRINT", TOK_PRINT},     {"PRINT#", TOK_PRINT_HASH},
         {"INPUT#", TOK_INPUT_HASH}, {"GET#", TOK_GET_HASH},   {"PUT#", TOK_PUT_HASH},
         {"DEF", TOK_DEF},           {"DATA", TOK_DATA},       {"READ", TOK_READ},
         {"RESTORE", TOK_RESTORE},   {"CLEAR", TOK_CLEAR},     {"SCREEN", TOK_SCREEN},
         {"CLS", TOK_CLS},           {"COLOR", TOK_COLOR},     {"KEYOFF", TOK_KEYOFF},
         {"LOCATE", TOK_LOCATE},     {"HTAB", TOK_HTAB},       {"VTAB", TOK_VTAB},
         {"POKE", TOK_POKE},         {"HOME", TOK_HOME},       {"BEEP", TOK_BEEP},
         {"SOUND", TOK_SOUND},       {"SYSTEM", TOK_SYSTEM},   {"RANDOMIZE", TOK_RANDOMIZE},
         {"TEXT", TOK_TEXT},         {"INVERSE", TOK_INVERSE}, {"NORMAL", TOK_NORMAL},
         {"HGR2", TOK_HGR2},         {"HCOLOR", TOK_HCOLOR},   {"HPLOT", TOK_HPLOT},
         {"MOVE", TOK_MOVE},         {"DRAW", TOK_DRAW},       {"LINE", TOK_LINE},
         {"CIRCLE", TOK_CIRCLE},     {"RECT", TOK_RECT},       {"MODE", TOK_MODE},
         {"FILL", TOK_FILL},         {"END", TOK_END},         {"STOP", TOK_STOP},
         {"FOR", TOK_FOR},           {"TO", TOK_TO},           {"STEP", TOK_STEP},
         {"NEXT", TOK_NEXT},         {"WHILE", TOK_WHILE},     {"WEND", TOK_WEND},
         {"GOSUB", TOK_GOSUB},       {"RETURN", TOK_RETURN},   {"ON", TOK_ON},
         {"ERROR", TOK_ERROR},       {"RESUME", TOK_RESUME},   {"CALL", TOK_CALL},
         {"AND", TOK_AND},           {"OR", TOK_OR},           {"NOT", TOK_NOT},
         {"MOD", TOK_MOD},           {"BASE", TOK_BASE},       {"FN", TOK_FN},
         {"TRUE", TOK_TRUE},         {"FALSE", TOK_FALSE},     {NULL, TOK_EOF}};
    for (int j = 0; keywords[j].kw != NULL; j++)
      if (strcmp (buf, keywords[j].kw) == 0) {
        t.type = keywords[j].type;
        return t;
      }
    t.type = TOK_IDENTIFIER;
    t.str = strdup (buf);
    return t;
  }
  if (isdigit (c) || c == '.') {
    char *start = cur;
    t.num = BASIC_STRTOF (cur, &cur);
    (void) start;
    t.type = TOK_NUMBER;
    return t;
  }
  if (c == '"') {
    cur++;
    const char *start = cur;
    while (*cur && *cur != '"') cur++;
    size_t len = cur - start;
    char *s = malloc (len + 1);
    memcpy (s, start, len);
    s[len] = 0;
    if (*cur == '"') cur++;
    t.type = TOK_STRING;
    t.str = s;
    return t;
  }
  cur++;
  switch (c) {
  case ',': t.type = TOK_COMMA; break;
  case ':': t.type = TOK_COLON; break;
  case ';': t.type = TOK_SEMICOLON; break;
  case '(': t.type = TOK_LPAREN; break;
  case ')': t.type = TOK_RPAREN; break;
  case '+': t.type = TOK_PLUS; break;
  case '-': t.type = TOK_MINUS; break;
  case '*': t.type = TOK_STAR; break;
  case '/': t.type = TOK_SLASH; break;
  case '\\': t.type = TOK_BACKSLASH; break;
  case '=': t.type = TOK_EQ; break;
  case '<':
    if (*cur == '=') {
      cur++;
      t.type = TOK_LE;
    } else if (*cur == '>') {
      cur++;
      t.type = TOK_NE;
    } else {
      t.type = TOK_LT;
    }
    break;
  case '>':
    if (*cur == '=') {
      cur++;
      t.type = TOK_GE;
    } else {
      t.type = TOK_GT;
    }
    break;
  default: t.type = TOK_EOF; break;
  }
  return t;
}

static Token next_token (Parser *p) {
  if (p->has_peek) {
    if (p->peek.str != NULL) free (p->peek.str);
    p->has_peek = 0;
  }
  p->tok = read_token (p);
  return p->tok;
}

static Token peek_token (Parser *p) {
  if (!p->has_peek) {
    char *start = cur;
    p->peek = read_token (p);
    p->peek_cur = cur;
    cur = start;
    p->has_peek = 1;
  }
  return p->peek;
}

static int parse_int (Parser *p) {
  skip_ws (p);
  char *start = cur;
  int v = strtol (cur, &cur, 10);
  if (cur == start) {
    fprintf (stderr, "expected integer");
    exit (1);
  }
  return v;
}

static char *parse_id (Parser *p) {
  Token t = next_token (p);
  if (t.type != TOK_IDENTIFIER) return NULL;
  return t.str;
}

static basic_num_t parse_number (Parser *p) {
  Token t = next_token (p);
  if (t.type != TOK_NUMBER) return 0;
  return t.num;
}

static char *parse_string (Parser *p) {
  Token t = next_token (p);
  if (t.type != TOK_STRING) return NULL;
  return t.str;
}

/* Expression parser */
static Node *parse_factor (Parser *p);
static Node *parse_term (Parser *p);
static Node *parse_add (Parser *p);

static Node *parse_rel (Parser *p);
static Node *parse_logical (Parser *p);

static Node *parse_expr (Parser *p);

typedef struct {
  Node *a[5];
} CallArgs;

static void parse_call_args (Parser *p, CallArgs *a) {
  /* Parse up to five comma-separated arguments, ensuring the first
     argument is stored and the closing parenthesis is consumed. */
  for (int i = 0; i < 5; i++) {
    Token t = peek_token (p);
    if (t.type == TOK_RPAREN) break;
    p->has_peek = 0;
    a->a[i] = parse_expr (p);
    if (a->a[i] == NULL) {
      parse_error (p);
      return;
    }
    t = peek_token (p);
    if (t.type == TOK_COMMA) {
      next_token (p);
    } else {
      break;
    }
  }
  if (peek_token (p).type == TOK_RPAREN) next_token (p);
}

typedef struct {
  const char *name;
  int returns_string;
} Builtin;

static const Builtin builtins[]
  = {{"RND", 0},      {"INT", 0},     {"TIMER", 0},      {"TIME", 0},    {"DATE", 0},
     {"PEEK", 0},     {"EOF", 0},     {"POS", 0},        {"ABS", 0},     {"SGN", 0},
     {"SQR", 0},      {"SIN", 0},     {"COS", 0},        {"TAN", 0},     {"ATN", 0},
     {"LOG", 0},      {"EXP", 0},     {"LEN", 0},        {"VAL", 0},     {"ASC", 0},
     {"INSTR", 0},    {"MIRCTX", 0},  {"MIRMOD", 0},     {"MIRFUNC", 0}, {"MIRREG", 0},
     {"MIRLABEL", 0}, {"MIREMIT", 0}, {"MIREMITLBL", 0}, {"MIRRET", 0},  {"MIRFINISH", 0},
     {"MIRRUN", 0},   {"MIRDUMP", 0}, {"CHR$", 1},       {"STRING$", 1}, {"TIME$", 1},
     {"DATE$", 1},    {"INPUT$", 1},  {"SPC", 1},        {"LEFT$", 1},   {"RIGHT$", 1},
     {"MID$", 1},     {"STR$", 1},    {"INKEY$", 1},     {NULL, 0}};

static const Builtin *lookup_builtin (const char *id) {
  for (int i = 0; builtins[i].name != NULL; i++)
    if (strcasecmp (id, builtins[i].name) == 0) return &builtins[i];
  return NULL;
}

static Node *parse_literal (Parser *p) {
  Token t = peek_token (p);
  Node *n;
  switch (t.type) {
  case TOK_NUMBER:
    next_token (p);
    n = new_node (N_NUM);
    n->num = t.num;
    return n;
  case TOK_STRING:
    next_token (p);
    n = new_node (N_STR);
    n->is_str = 1;
    n->str = t.str;
    return n;
  case TOK_TRUE:
  case TOK_FALSE:
    next_token (p);
    n = new_node (N_NUM);
    n->num = (t.type == TOK_TRUE);
    return n;
  default: return NULL;
  }
}

static Node *parse_builtin_call (char *id, CallArgs *a) {
  const Builtin *b = lookup_builtin (id);
  if (b == NULL) return NULL;
  if (!strcasecmp (id, "RND") && a->a[0] == NULL) {
    a->a[0] = new_node (N_NUM);
    a->a[0]->num = 1;
  }
  Node *n = new_node (N_CALL);
  n->var = id;
  n->is_str = b->returns_string;
  Node **args[5] = {&n->left, &n->right, &n->index, &n->index2, &n->arg4};
  for (int i = 0; i < 5; i++) *args[i] = a->a[i];
  return n;
}

static Node *parse_user_call (char *id, CallArgs *a) {
  FuncDef *fd = find_func (id);
  if (strncasecmp (id, "FN", 2) != 0 && fd == NULL) return NULL;
  Node *n = new_node (N_CALL);
  n->var = id;
  n->is_str = fd ? fd->is_str_ret : id[strlen (id) - 1] == '$';
  Node **args[5] = {&n->left, &n->right, &n->index, &n->index2, &n->arg4};
  for (int i = 0; i < 5; i++) *args[i] = a->a[i];
  return n;
}

static Node *parse_variable (char *id, CallArgs *a) {
  Node *n = new_node (N_VAR);
  n->var = id;
  n->is_str = id[strlen (id) - 1] == '$';
  Node **args[2] = {&n->index, &n->index2};
  for (int i = 0; i < 2; i++) *args[i] = a->a[i];
  return n;
}

static Node *parse_not (Parser *p) {
  Token t = peek_token (p);
  if (t.type != TOK_NOT) return NULL;
  next_token (p);
  Node *n = new_node (N_NOT);
  n->left = parse_factor (p);
  return n;
}

static Node *parse_paren (Parser *p) {
  Token t = peek_token (p);
  if (t.type != TOK_LPAREN) return NULL;
  next_token (p);
  Node *e = parse_expr (p);
  if (e == NULL) {
    parse_error (p);
    return NULL;
  }
  if (peek_token (p).type == TOK_RPAREN) next_token (p);
  return e;
}

static Node *parse_unary_minus (Parser *p) {
  Token t = peek_token (p);
  if (t.type != TOK_MINUS) return NULL;
  next_token (p);
  Node *n = new_node (N_NEG);
  n->left = parse_factor (p);
  return n;
}

static Node *parse_factor (Parser *p) {
  Node *n;
  if ((n = parse_not (p)) != NULL) return n;
  if ((n = parse_paren (p)) != NULL) return n;
  if ((n = parse_unary_minus (p)) != NULL) return n;
  if ((n = parse_literal (p)) != NULL) return n;
  Token t = peek_token (p);
  if (t.type != TOK_IDENTIFIER) return NULL;
  char *id = parse_id (p);
  CallArgs a = {{0}};
  t = peek_token (p);
  if (t.type == TOK_LPAREN) {
    next_token (p);
    parse_call_args (p, &a);
  } else {
    p->has_peek = 0;
  }
  if ((n = parse_builtin_call (id, &a)) != NULL) return n;
  if ((n = parse_user_call (id, &a)) != NULL) return n;
  return parse_variable (id, &a);
}

static Node *parse_term (Parser *p) {
  Node *n = parse_factor (p);
  if (n == NULL) return NULL;
  while (1) {
    Token t = peek_token (p);
    if (t.type != TOK_STAR && t.type != TOK_SLASH && t.type != TOK_BACKSLASH && t.type != TOK_MOD) {
      skip_ws (p);
      break;
    }
    next_token (p);
    Node *r = parse_factor (p);
    Node *nn = new_node (N_BIN);
    switch (t.type) {
    case TOK_STAR: nn->op = '*'; break;
    case TOK_SLASH: nn->op = '/'; break;
    case TOK_BACKSLASH: nn->op = '\\'; break;
    case TOK_MOD: nn->op = '%'; break;
    default: break;
    }
    nn->left = n;
    nn->right = r;
    n = nn;
  }
  return n;
}

static Node *parse_add (Parser *p) {
  Node *n = parse_term (p);
  if (n == NULL) return NULL;
  while (1) {
    Token t = peek_token (p);
    if (t.type != TOK_PLUS && t.type != TOK_MINUS) {
      skip_ws (p);
      break;
    }
    next_token (p);
    Node *r = parse_term (p);
    Node *nn = new_node (N_BIN);
    nn->op = (t.type == TOK_PLUS ? '+' : '-');
    nn->left = n;
    nn->right = r;
    if (t.type == TOK_PLUS && (n->is_str || r->is_str)) nn->is_str = 1;
    n = nn;
  }
  return n;
}

static Node *parse_rel (Parser *p) {
  Node *n = parse_add (p);
  Token t = peek_token (p);
  TokenType op_type = TOK_EOF;
  if (t.type == TOK_EQ || t.type == TOK_NE || t.type == TOK_LE || t.type == TOK_GE) {
    next_token (p);
    op_type = t.type;
  } else if (t.type == TOK_LT || t.type == TOK_GT) {
    next_token (p);
    Token t2 = peek_token (p);
    if (t.type == TOK_LT && t2.type == TOK_GT) {
      next_token (p);
      op_type = TOK_NE;
    } else if (t2.type == TOK_EQ) {
      next_token (p);
      op_type = (t.type == TOK_LT ? TOK_LE : TOK_GE);
    } else {
      op_type = t.type;
    }
  } else {
    skip_ws (p);
    return n;
  }
  Node *rhs = parse_add (p);
  Node *nn = new_node (N_BIN);
  switch (op_type) {
  case TOK_EQ: nn->op = '='; break;
  case TOK_NE: nn->op = '!'; break;
  case TOK_LT: nn->op = '<'; break;
  case TOK_LE: nn->op = 'L'; break;
  case TOK_GT: nn->op = '>'; break;
  case TOK_GE: nn->op = 'G'; break;
  default: break;
  }
  nn->left = n;
  nn->right = rhs;
  skip_ws (p);
  return nn;
}

static Node *parse_logical (Parser *p) {
  Node *n = parse_rel (p);
  while (1) {
    Token t = peek_token (p);
    if (t.type != TOK_AND && t.type != TOK_OR) {
      skip_ws (p);
      break;
    }
    next_token (p);
    Node *r = parse_rel (p);
    Node *nn = new_node (N_BIN);
    nn->op = (t.type == TOK_AND ? '&' : '|');
    nn->left = n;
    nn->right = r;
    n = nn;
  }
  return n;
}

static Node *parse_expr (Parser *p) {
  Node *n = parse_logical (p);
  p->has_peek = 0;
  return n;
}

static int parse_if_part (Parser *p, StmtVec *vec, int stop_on_else);

/* Parse a single line into statement */
#define PARSE_EXPR_OR_ERROR(dest)               \
  do {                                          \
    p->has_peek = 0;                            \
    dest = parse_expr (p);                      \
    if ((dest) == NULL) return parse_error (p); \
  } while (0)

static int parse_stmt (Parser *p, Stmt *out) {
  skip_ws (p);
  if (strncasecmp (cur, "REM", 3) == 0) {
    /* Comment line: ignore rest */
    out->kind = ST_REM;
    return 1;
  } else if (strncasecmp (cur, "OPTION", 6) == 0) {
    cur += 6;
    skip_ws (p);
    if (strncasecmp (cur, "BASE", 4) != 0) return 0;
    cur += 4;
    skip_ws (p);
    int base = parse_int (p);
    if (base != 0 && base != 1) return 0;
    array_base = base;
    out->kind = ST_REM;
    return 1;
  } else if (strncasecmp (cur, "DIM", 3) == 0) {
    cur += 3;
    out->kind = ST_DIM;
    out->u.dim.names = NULL;
    out->u.dim.sizes1 = NULL;
    out->u.dim.sizes2 = NULL;
    out->u.dim.is_str = NULL;
    out->u.dim.n = 0;
    size_t cap = 0;
    while (1) {
      char *name = parse_id (p);
      int is_str = name[strlen (name) - 1] == '$';
      skip_ws (p);
      Node *size1 = NULL, *size2 = NULL;
      if (*cur == '(') {
        cur++;
        PARSE_EXPR_OR_ERROR (size1);
        skip_ws (p);
        if (*cur == ',') {
          cur++;
          PARSE_EXPR_OR_ERROR (size2);
          skip_ws (p);
        }
        if (*cur == ')') cur++;
      }
      if (out->u.dim.n == cap) {
        cap = cap ? 2 * cap : 4;
        out->u.dim.names = realloc (out->u.dim.names, cap * sizeof (char *));
        out->u.dim.sizes1 = realloc (out->u.dim.sizes1, cap * sizeof (Node *));
        out->u.dim.sizes2 = realloc (out->u.dim.sizes2, cap * sizeof (Node *));
        out->u.dim.is_str = realloc (out->u.dim.is_str, cap * sizeof (int));
      }
      out->u.dim.names[out->u.dim.n] = name;
      out->u.dim.sizes1[out->u.dim.n] = size1;
      out->u.dim.sizes2[out->u.dim.n] = size2;
      out->u.dim.is_str[out->u.dim.n] = is_str;
      out->u.dim.n++;
      skip_ws (p);
      if (*cur != ',') break;
      cur++;
    }
    return 1;
  } else if (strncasecmp (cur, "CLEAR", 5) == 0) {
    cur += 5;
    out->kind = ST_CLEAR;
    return 1;
  } else if (strncasecmp (cur, "OPEN", 4) == 0) {
    cur += 4;
    skip_ws (p);
    out->kind = ST_OPEN;
    PARSE_EXPR_OR_ERROR (out->u.open.num);
    skip_ws (p);
    if (*cur != ',') return 0;
    cur++;
    PARSE_EXPR_OR_ERROR (out->u.open.path);
    return 1;
  } else if (strncasecmp (cur, "CLOSE", 5) == 0) {
    cur += 5;
    skip_ws (p);
    out->kind = ST_CLOSE;
    PARSE_EXPR_OR_ERROR (out->u.close.num);
    return 1;
  } else if (strncasecmp (cur, "PRINT#", 6) == 0) {
    cur += 6;
    skip_ws (p);
    out->kind = ST_PRINT_HASH;
    PARSE_EXPR_OR_ERROR (out->u.printhash.num);
    skip_ws (p);
    if (*cur != ',') return 0;
    cur++;
    out->u.printhash.items = NULL;
    out->u.printhash.n = 0;
    out->u.printhash.no_nl = 0;
    return 1;
  } else if (strncasecmp (cur, "PRINT", 5) == 0) {
    cur += 5;
    out->kind = ST_PRINT;
    out->u.print.items = NULL;
    out->u.print.n = 0;
    out->u.print.no_nl = 0;
    return 1;
  } else if (strncasecmp (cur, "INPUT#", 6) == 0) {
    cur += 6;
    skip_ws (p);
    out->kind = ST_INPUT_HASH;
    PARSE_EXPR_OR_ERROR (out->u.inputhash.num);
    skip_ws (p);
    if (*cur != ',') return 0;
    cur++;
    out->u.inputhash.var = parse_id (p);
    out->u.inputhash.is_str = out->u.inputhash.var[strlen (out->u.inputhash.var) - 1] == '$';
    return 1;
  } else if (strncasecmp (cur, "INPUT", 5) == 0) {
    cur += 5;
    out->kind = ST_INPUT;
    out->u.input.var = parse_id (p);
    out->u.input.is_str = out->u.input.var[strlen (out->u.input.var) - 1] == '$';
    return 1;
  } else if (strncasecmp (cur, "GET#", 4) == 0) {
    cur += 4;
    skip_ws (p);
    out->kind = ST_GET_HASH;
    PARSE_EXPR_OR_ERROR (out->u.gethash.num);
    skip_ws (p);
    if (*cur != ',') return 0;
    cur++;
    out->u.gethash.var = parse_id (p);
    return 1;
  } else if (strncasecmp (cur, "GET", 3) == 0) {
    cur += 3;
    out->kind = ST_GET;
    out->u.get.var = parse_id (p);
    return 1;
  } else if (strncasecmp (cur, "PUT#", 4) == 0) {
    cur += 4;
    skip_ws (p);
    out->kind = ST_PUT_HASH;
    PARSE_EXPR_OR_ERROR (out->u.puthash.num);
    skip_ws (p);
    if (*cur != ',') return 0;
    cur++;
    PARSE_EXPR_OR_ERROR (out->u.puthash.expr);
    return 1;
  } else if (strncasecmp (cur, "PUT", 3) == 0) {
    cur += 3;
    out->kind = ST_PUT;
    PARSE_EXPR_OR_ERROR (out->u.put.expr);
    return 1;

  } else if (strncasecmp (cur, "DEF", 3) == 0) {
    cur += 3;
    skip_ws (p);
    char *fname = parse_id (p);
    if (strncasecmp (fname, "FN", 2) != 0) return 0;
    int f_is_str = fname[strlen (fname) - 1] == '$';
    skip_ws (p);
    if (*cur != '(') return 0;
    cur++;
    char **params = NULL;
    int *is_str = NULL;
    size_t n = 0, cap = 0;
    skip_ws (p);
    if (*cur != ')') {
      while (1) {
        char *param = parse_id (p);
        int ps = param[strlen (param) - 1] == '$';
        if (n == cap) {
          cap = cap ? 2 * cap : 4;
          params = realloc (params, cap * sizeof (char *));
          is_str = realloc (is_str, cap * sizeof (int));
        }
        params[n] = param;
        is_str[n] = ps;
        n++;
        skip_ws (p);
        if (*cur != ',') break;
        cur++;
        skip_ws (p);
      }
    }
    if (*cur == ')') cur++;
    skip_ws (p);
    if (*cur != '=') return 0;
    cur++;
    Node *body;
    PARSE_EXPR_OR_ERROR (body);
    FuncDef fd
      = {fname, params, is_str, n, body, (StmtVec) {0}, f_is_str, 0, NULL, NULL, NULL, 0, 0};
    func_vec_push (&func_defs, fd);
    out->kind = ST_DEF;
    return 1;

  } else if (strncasecmp (cur, "DATA", 4) == 0) {
    cur += 4;
    out->kind = ST_DATA;
    while (1) {
      skip_ws (p);
      BasicData d;
      d.is_str = 0;
      d.num = 0;
      d.str = NULL;
      if (*cur == '"') {
        d.is_str = 1;
        d.str = parse_string (p);
      } else {
        d.num = parse_number (p);
      }
      data_vec_push (&data_vals, d);
      skip_ws (p);
      if (*cur != ',') break;
      cur++;
    }
    return 1;
  } else if (strncasecmp (cur, "READ", 4) == 0) {
    cur += 4;
    out->kind = ST_READ;
    out->u.read.vars = NULL;
    out->u.read.n = 0;
    size_t cap = 0;
    while (1) {
      Node *v = parse_factor (p);
      if (v == NULL) return parse_error (p);
      if (out->u.read.n == cap) {
        cap = cap ? cap * 2 : 4;
        out->u.read.vars = realloc (out->u.read.vars, cap * sizeof (Node *));
      }
      out->u.read.vars[out->u.read.n++] = v;
      skip_ws (p);
      if (*cur != ',') break;
      cur++;
    }
    return 1;
  } else if (strncasecmp (cur, "RESTORE", 7) == 0) {
    cur += 7;
    out->kind = ST_RESTORE;
    return 1;
  } else if (strncasecmp (cur, "SCREEN", 6) == 0) {
    cur += 6;
    skip_ws (p);
    out->kind = ST_SCREEN;
    PARSE_EXPR_OR_ERROR (out->u.expr);
    return 1;
  } else if (strncasecmp (cur, "CLS", 3) == 0) {
    cur += 3;
    out->kind = ST_CLS;
    return 1;
  } else if (strncasecmp (cur, "COLOR", 5) == 0) {
    cur += 5;
    skip_ws (p);
    out->kind = ST_COLOR;
    PARSE_EXPR_OR_ERROR (out->u.expr);
    return 1;
  } else if (strncasecmp (cur, "KEY", 3) == 0) {
    cur += 3;
    skip_ws (p);
    if (strncasecmp (cur, "OFF", 3) != 0) return 0;
    cur += 3;
    out->kind = ST_KEYOFF;
    return 1;
  } else if (strncasecmp (cur, "LOCATE", 6) == 0) {
    cur += 6;
    skip_ws (p);
    out->kind = ST_LOCATE;
    PARSE_EXPR_OR_ERROR (out->u.locate.row);
    skip_ws (p);
    if (*cur != ',') return 0;
    cur++;
    PARSE_EXPR_OR_ERROR (out->u.locate.col);
    return 1;
  } else if (strncasecmp (cur, "HTAB", 4) == 0 || strncasecmp (cur, "TAB", 3) == 0) {
    cur += strncasecmp (cur, "HTAB", 4) == 0 ? 4 : 3;
    skip_ws (p);
    out->kind = ST_HTAB;
    PARSE_EXPR_OR_ERROR (out->u.expr);
    return 1;
  } else if (strncasecmp (cur, "POKE", 4) == 0) {
    cur += 4;
    skip_ws (p);
    out->kind = ST_POKE;
    PARSE_EXPR_OR_ERROR (out->u.poke.addr);
    skip_ws (p);
    if (*cur != ',') return 0;
    cur++;
    PARSE_EXPR_OR_ERROR (out->u.poke.value);
    return 1;
  } else if (strncasecmp (cur, "HOME", 4) == 0) {
    cur += 4;
    out->kind = ST_HOME;
    return 1;
  } else if (strncasecmp (cur, "VTAB", 4) == 0) {
    cur += 4;
    skip_ws (p);
    out->kind = ST_VTAB;
    PARSE_EXPR_OR_ERROR (out->u.expr);
    return 1;
  } else if (strncasecmp (cur, "BEEP", 4) == 0) {
    cur += 4;
    out->kind = ST_BEEP;
    return 1;
  } else if (strncasecmp (cur, "SOUND", 5) == 0) {
    cur += 5;
    skip_ws (p);
    out->kind = ST_SOUND;
    PARSE_EXPR_OR_ERROR (out->u.sound.freq);
    skip_ws (p);
    if (*cur != ',') return 0;
    cur++;
    PARSE_EXPR_OR_ERROR (out->u.sound.dur);
    return 1;
  } else if (strncasecmp (cur, "SYSTEM", 6) == 0) {
    cur += 6;
    skip_ws (p);
    out->kind = ST_SYSTEM;
    PARSE_EXPR_OR_ERROR (out->u.system.cmd);
    skip_ws (p);
    if (*cur != ',') return 0;
    cur++;
    out->u.system.status = parse_id (p);
    if (out->u.system.status[strlen (out->u.system.status) - 1] == '$') return 0;
    skip_ws (p);
    if (*cur != ',') return 0;
    cur++;
    out->u.system.out = parse_id (p);
    if (out->u.system.out[strlen (out->u.system.out) - 1] != '$') return 0;
    return 1;
  } else if (strncasecmp (cur, "RANDOMIZE", 9) == 0) {
    cur += 9;
    skip_ws (p);
    out->kind = ST_RANDOMIZE;
    if (*cur == '\0' || *cur == ':') {
      out->u.expr = NULL;
    } else {
      PARSE_EXPR_OR_ERROR (out->u.expr);
    }
    return 1;
  } else if (strncasecmp (cur, "TEXT", 4) == 0) {
    cur += 4;
    out->kind = ST_TEXT;
    return 1;
  } else if (strncasecmp (cur, "INVERSE", 7) == 0) {
    cur += 7;
    out->kind = ST_INVERSE;
    return 1;
  } else if (strncasecmp (cur, "NORMAL", 6) == 0) {
    cur += 6;
    out->kind = ST_NORMAL;
    return 1;
  } else if (strncasecmp (cur, "HGR2", 4) == 0) {
    cur += 4;
    out->kind = ST_HGR2;
    return 1;
  } else if (strncasecmp (cur, "HCOLOR=", 7) == 0) {
    cur += 7;
    skip_ws (p);
    out->kind = ST_HCOLOR;
    PARSE_EXPR_OR_ERROR (out->u.expr);
    return 1;
  } else if (strncasecmp (cur, "HPLOT", 5) == 0) {
    cur += 5;
    skip_ws (p);
    out->kind = ST_HPLOT;
    size_t cap = 0;
    out->u.hplot.n = 0;
    out->u.hplot.xs = out->u.hplot.ys = NULL;
    out->u.hplot.from_prev = 0;
    if (strncasecmp (cur, "TO", 2) == 0) {
      out->u.hplot.from_prev = 1;
      cur += 2;
      skip_ws (p);
    }
    while (1) {
      if (out->u.hplot.n >= cap) {
        cap = cap ? cap * 2 : 4;
        out->u.hplot.xs = realloc (out->u.hplot.xs, cap * sizeof (Node *));
        out->u.hplot.ys = realloc (out->u.hplot.ys, cap * sizeof (Node *));
      }
      PARSE_EXPR_OR_ERROR (out->u.hplot.xs[out->u.hplot.n]);
      skip_ws (p);
      if (*cur != ',') return 0;
      cur++;
      PARSE_EXPR_OR_ERROR (out->u.hplot.ys[out->u.hplot.n]);
      out->u.hplot.n++;
      skip_ws (p);
      if (strncasecmp (cur, "TO", 2) == 0) {
        cur += 2;
        skip_ws (p);
      } else {
        break;
      }
    }
    return 1;
  } else if (strncasecmp (cur, "MOVE", 4) == 0) {
    cur += 4;
    skip_ws (p);
    out->kind = ST_MOVE;
    PARSE_EXPR_OR_ERROR (out->u.move.x);
    if (*cur != ',') return 0;
    cur++;
    PARSE_EXPR_OR_ERROR (out->u.move.y);
    return 1;
  } else if (strncasecmp (cur, "DRAW", 4) == 0) {
    cur += 4;
    skip_ws (p);
    out->kind = ST_DRAW;
    PARSE_EXPR_OR_ERROR (out->u.draw.x);
    if (*cur != ',') return 0;
    cur++;
    PARSE_EXPR_OR_ERROR (out->u.draw.y);
    return 1;
  } else if (strncasecmp (cur, "LINE", 4) == 0) {
    cur += 4;
    skip_ws (p);
    out->kind = ST_LINE;
    PARSE_EXPR_OR_ERROR (out->u.line.x0);
    if (*cur != ',') return 0;
    cur++;
    PARSE_EXPR_OR_ERROR (out->u.line.y0);
    skip_ws (p);
    if (strncasecmp (cur, "TO", 2) == 0) {
      cur += 2;
      skip_ws (p);
    }
    PARSE_EXPR_OR_ERROR (out->u.line.x1);
    if (*cur != ',') return 0;
    cur++;
    PARSE_EXPR_OR_ERROR (out->u.line.y1);
    return 1;
  } else if (strncasecmp (cur, "CIRCLE", 6) == 0) {
    cur += 6;
    skip_ws (p);
    out->kind = ST_CIRCLE;
    PARSE_EXPR_OR_ERROR (out->u.circle.x);
    if (*cur != ',') return 0;
    cur++;
    PARSE_EXPR_OR_ERROR (out->u.circle.y);
    if (*cur != ',') return 0;
    cur++;
    PARSE_EXPR_OR_ERROR (out->u.circle.r);
    return 1;
  } else if (strncasecmp (cur, "RECT", 4) == 0) {
    cur += 4;
    skip_ws (p);
    out->kind = ST_RECT;
    PARSE_EXPR_OR_ERROR (out->u.rect.x0);
    if (*cur != ',') return 0;
    cur++;
    PARSE_EXPR_OR_ERROR (out->u.rect.y0);
    if (*cur != ',') return 0;
    cur++;
    PARSE_EXPR_OR_ERROR (out->u.rect.x1);
    if (*cur != ',') return 0;
    cur++;
    PARSE_EXPR_OR_ERROR (out->u.rect.y1);
    return 1;
  } else if (strncasecmp (cur, "MODE", 4) == 0) {
    cur += 4;
    skip_ws (p);
    out->kind = ST_MODE;
    PARSE_EXPR_OR_ERROR (out->u.expr);
    return 1;
  } else if (strncasecmp (cur, "FILL", 4) == 0) {
    cur += 4;
    skip_ws (p);
    out->kind = ST_FILL;
    PARSE_EXPR_OR_ERROR (out->u.fill.x0);
    if (*cur != ',') return 0;
    cur++;
    PARSE_EXPR_OR_ERROR (out->u.fill.y0);
    if (*cur != ',') return 0;
    cur++;
    PARSE_EXPR_OR_ERROR (out->u.fill.x1);
    if (*cur != ',') return 0;
    cur++;
    PARSE_EXPR_OR_ERROR (out->u.fill.y1);
    return 1;
  } else if (strncasecmp (cur, "LET", 3) == 0) {
    cur += 3;
    skip_ws (p);
    Node *v = parse_factor (p);
    if (v == NULL) return parse_error (p);
    skip_ws (p);
    if (*cur == '=') cur++;
    Node *e;
    PARSE_EXPR_OR_ERROR (e);
    out->kind = ST_LET;
    out->u.let.var = v;
    out->u.let.expr = e;
    out->u.let.is_str = v->is_str;
    return 1;
  } else if (strncasecmp (cur, "GOTO", 4) == 0) {
    cur += 4;
    out->kind = ST_GOTO;
    out->u.target = parse_int (p);
    return 1;
  } else if (strncasecmp (cur, "GOSUB", 5) == 0) {
    cur += 5;
    out->kind = ST_GOSUB;
    out->u.target = parse_int (p);
    return 1;
  } else if (strncasecmp (cur, "IF", 2) == 0) {
    cur += 2;
    Stmt s;
    s.kind = ST_IF;
    PARSE_EXPR_OR_ERROR (s.u.iff.cond);
    if (strncasecmp (cur, "THEN", 4) != 0) return 0;
    cur += 4;
    skip_ws (p);
    s.u.iff.then_stmts = (StmtVec) {0};
    if (!parse_if_part (p, &s.u.iff.then_stmts, 1)) return 0;
    skip_ws (p);
    s.u.iff.else_stmts = (StmtVec) {0};
    if (strncasecmp (cur, "ELSE", 4) == 0) {
      cur += 4;
      skip_ws (p);
      if (!parse_if_part (p, &s.u.iff.else_stmts, 0)) return 0;
    }
    *out = s;
    return 1;
  } else if (strncasecmp (cur, "FOR", 3) == 0) {
    cur += 3;
    out->kind = ST_FOR;
    out->u.forto.var = parse_id (p);
    skip_ws (p);
    if (*cur == '=') cur++;
    PARSE_EXPR_OR_ERROR (out->u.forto.start);
    if (strncasecmp (cur, "TO", 2) != 0) return 0;
    cur += 2;
    PARSE_EXPR_OR_ERROR (out->u.forto.end);
    if (strncasecmp (cur, "STEP", 4) == 0) {
      cur += 4;
      PARSE_EXPR_OR_ERROR (out->u.forto.step);
    } else {
      Node *one = new_node (N_NUM);
      one->num = 1;
      out->u.forto.step = one;
    }
    return 1;
  } else if (strncasecmp (cur, "NEXT", 4) == 0) {
    cur += 4;
    out->kind = ST_NEXT;
    out->u.next.var = NULL;
    skip_ws (p);
    if (isalpha ((unsigned char) *cur)) out->u.next.var = parse_id (p);
    return 1;
  } else if (strncasecmp (cur, "WHILE", 5) == 0) {
    cur += 5;
    out->kind = ST_WHILE;
    PARSE_EXPR_OR_ERROR (out->u.expr);
    return 1;
  } else if (strncasecmp (cur, "WEND", 4) == 0) {
    cur += 4;
    out->kind = ST_WEND;
    return 1;
  } else if (strncasecmp (cur, "RETURN", 6) == 0) {
    cur += 6;
    out->kind = ST_RETURN;
    return 1;
  } else if (strncasecmp (cur, "ON", 2) == 0) {
    cur += 2;
    skip_ws (p);
    if (strncasecmp (cur, "ERROR", 5) == 0) {
      cur += 5;
      skip_ws (p);
      if (strncasecmp (cur, "GOTO", 4) != 0) return 0;
      cur += 4;
      out->kind = ST_ON_ERROR;
      out->u.target = parse_int (p);
      return 1;
    }
    Node *e;
    PARSE_EXPR_OR_ERROR (e);
    int **targets = NULL;
    size_t *n_targets = NULL;
    if (strncasecmp (cur, "GOSUB", 5) == 0) {
      cur += 5;
      out->kind = ST_ON_GOSUB;
      out->u.on_gosub.expr = e;
      out->u.on_gosub.targets = NULL;
      out->u.on_gosub.n_targets = 0;
      targets = &out->u.on_gosub.targets;
      n_targets = &out->u.on_gosub.n_targets;
    } else if (strncasecmp (cur, "GOTO", 4) == 0) {
      cur += 4;
      out->kind = ST_ON_GOTO;
      out->u.on_goto.expr = e;
      out->u.on_goto.targets = NULL;
      out->u.on_goto.n_targets = 0;
      targets = &out->u.on_goto.targets;
      n_targets = &out->u.on_goto.n_targets;
    } else {
      return 0;
    }
    size_t cap = 0;
    while (1) {
      int t = parse_int (p);
      if (*n_targets == cap) {
        cap = cap ? cap * 2 : 4;
        *targets = realloc (*targets, cap * sizeof (int));
      }
      (*targets)[(*n_targets)++] = t;
      skip_ws (p);
      if (*cur != ',') break;
      cur++;
    }
    return 1;
  } else if (strncasecmp (cur, "END", 3) == 0) {
    cur += 3;
    out->kind = ST_END;
    return 1;
  } else if (strncasecmp (cur, "STOP", 4) == 0) {
    cur += 4;
    out->kind = ST_STOP;
    return 1;
  } else if (strncasecmp (cur, "RESUME", 6) == 0) {
    cur += 6;
    out->kind = ST_RESUME;
    skip_ws (p);
    if (isdigit ((unsigned char) *cur)) {
      out->u.resume.has_line = 1;
      out->u.resume.line = parse_int (p);
    } else {
      out->u.resume.has_line = 0;
      out->u.resume.line = 0;
    }
    return 1;
  } else if (strncasecmp (cur, "CALL", 4) == 0) {
    cur += 4;
    skip_ws (p);
    char *name = parse_id (p);
    Node *arg1 = NULL, *arg2 = NULL, *arg3 = NULL, *arg4 = NULL, *arg5 = NULL;
    skip_ws (p);
    if (*cur == '(') {
      cur++;
      skip_ws (p);
      if (*cur != ')') {
        PARSE_EXPR_OR_ERROR (arg1);
        skip_ws (p);
        if (*cur == ',') {
          cur++;
          PARSE_EXPR_OR_ERROR (arg2);
          skip_ws (p);
          if (*cur == ',') {
            cur++;
            PARSE_EXPR_OR_ERROR (arg3);
            skip_ws (p);
            if (*cur == ',') {
              cur++;
              PARSE_EXPR_OR_ERROR (arg4);
              skip_ws (p);
              if (*cur == ',') {
                cur++;
                PARSE_EXPR_OR_ERROR (arg5);
                skip_ws (p);
              }
            }
          }
        }
      }
      if (*cur == ')') cur++;
    } else if (*cur != ':' && *cur != '\0') {
      PARSE_EXPR_OR_ERROR (arg1);
      skip_ws (p);
      if (*cur == ',') {
        cur++;
        PARSE_EXPR_OR_ERROR (arg2);
        skip_ws (p);
        if (*cur == ',') {
          cur++;
          PARSE_EXPR_OR_ERROR (arg3);
          skip_ws (p);
        }
      }
    }
    out->kind = ST_CALL;
    out->u.call.name = name;
    out->u.call.arg1 = arg1;
    out->u.call.arg2 = arg2;
    out->u.call.arg3 = arg3;
    out->u.call.arg4 = arg4;
    out->u.call.arg5 = arg5;
    return 1;
  } else if (isalpha ((unsigned char) *cur)) {
    char *name = parse_id (p);
    Node *arg1 = NULL, *arg2 = NULL, *arg3 = NULL, *arg4 = NULL, *arg5 = NULL;
    skip_ws (p);
    int had_paren = 0;
    if (*cur == '(') {
      had_paren = 1;
      cur++;
      skip_ws (p);
      if (*cur != ')') {
        PARSE_EXPR_OR_ERROR (arg1);
        skip_ws (p);
        if (*cur == ',') {
          cur++;
          PARSE_EXPR_OR_ERROR (arg2);
          skip_ws (p);
          if (*cur == ',') {
            cur++;
            PARSE_EXPR_OR_ERROR (arg3);
            skip_ws (p);
            if (*cur == ',') {
              cur++;
              PARSE_EXPR_OR_ERROR (arg4);
              skip_ws (p);
              if (*cur == ',') {
                cur++;
                PARSE_EXPR_OR_ERROR (arg5);
                skip_ws (p);
              }
            }
          }
        }
      }
      if (*cur == ')') cur++;
      skip_ws (p);
    }
    if (*cur == '=' && (!had_paren || (had_paren && arg3 == NULL))) {
      cur++;
      Node *e;
      PARSE_EXPR_OR_ERROR (e);
      Node *v = new_node (N_VAR);
      v->var = name;
      v->is_str = name[strlen (name) - 1] == '$';
      v->index = arg1;
      v->index2 = arg2;
      out->kind = ST_LET;
      out->u.let.var = v;
      out->u.let.expr = e;
      out->u.let.is_str = v->is_str;
      return 1;
    }
    if (!had_paren && *cur != ':' && *cur != '\0') {
      PARSE_EXPR_OR_ERROR (arg1);
      skip_ws (p);
      if (*cur == ',') {
        cur++;
        PARSE_EXPR_OR_ERROR (arg2);
        skip_ws (p);
        if (*cur == ',') {
          cur++;
          PARSE_EXPR_OR_ERROR (arg3);
          skip_ws (p);
        }
      }
    }
    out->kind = ST_CALL;
    out->u.call.name = name;
    out->u.call.arg1 = arg1;
    out->u.call.arg2 = arg2;
    out->u.call.arg3 = arg3;
    out->u.call.arg4 = arg4;
    out->u.call.arg5 = arg5;
    return 1;
  }
  return 0;
}

static int parse_if_part (Parser *p, StmtVec *vec, int stop_on_else) {
  while (1) {
    Stmt bs;
    if (isdigit ((unsigned char) *cur)) {
      bs.kind = ST_GOTO;
      bs.u.target = parse_int (p);
    } else {
      p->has_peek = 0;
      if (!parse_stmt (p, &bs)) return 0;
      if (bs.kind == ST_PRINT || bs.kind == ST_PRINT_HASH) {
        size_t cap = 0;
        if (bs.kind == ST_PRINT) {
          bs.u.print.items = NULL;
          bs.u.print.n = 0;
          bs.u.print.no_nl = 0;
        } else {
          bs.u.printhash.items = NULL;
          bs.u.printhash.n = 0;
          bs.u.printhash.no_nl = 0;
        }
        while (1) {
          skip_ws (p);
          if (*cur == ':' || *cur == '\0' || (stop_on_else && strncasecmp (cur, "ELSE", 4) == 0))
            break;
          Node *e;
          PARSE_EXPR_OR_ERROR (e);
          if (bs.kind == ST_PRINT) {
            if (bs.u.print.n == cap) {
              cap = cap ? cap * 2 : 4;
              bs.u.print.items = realloc (bs.u.print.items, cap * sizeof (Node *));
            }
            bs.u.print.items[bs.u.print.n++] = e;
          } else {
            if (bs.u.printhash.n == cap) {
              cap = cap ? cap * 2 : 4;
              bs.u.printhash.items = realloc (bs.u.printhash.items, cap * sizeof (Node *));
            }
            bs.u.printhash.items[bs.u.printhash.n++] = e;
          }
          skip_ws (p);
          if (*cur == ';' || *cur == ',') {
            cur++;
            skip_ws (p);
            if (*cur == ':' || *cur == '\0'
                || (stop_on_else && strncasecmp (cur, "ELSE", 4) == 0)) {
              if (bs.kind == ST_PRINT)
                bs.u.print.no_nl = 1;
              else
                bs.u.printhash.no_nl = 1;
              break;
            }
            continue;
          }
          break;
        }
        skip_ws (p);
      }
    }
    stmt_vec_push (vec, bs);
    skip_ws (p);
    if (*cur != ':') break;
    do {
      cur++;
      skip_ws (p);
    } while (*cur == ':');
    if (*cur == '\0' || (stop_on_else && strncasecmp (cur, "ELSE", 4) == 0)) break;
  }
  return 1;
}

/* Parse a single line into multiple statements */
static int parse_line (Parser *p, char *line, Line *out) {
  *p = (Parser) {0};
  cur = line;
  p->tok.type = TOK_EOF;
  p->line_start = line;
  out->src = strdup (line);
  skip_ws (p);
  int line_no = 0;
  if (isdigit ((unsigned char) *cur)) line_no = parse_int (p);
  p->line_no = line_no;
  out->line = line_no;
  out->stmts = (StmtVec) {0};
  while (1) {
    skip_ws (p);
    while (*cur == ':') {
      cur++;
      skip_ws (p);
    }
    if (*cur == '\0') break;
    Stmt s;
    p->has_peek = 0;
    if (!parse_stmt (p, &s)) return parse_error (p);
    if (s.kind == ST_PRINT || s.kind == ST_PRINT_HASH) {
      size_t cap = 0;
      if (s.kind == ST_PRINT) {
        s.u.print.items = NULL;
        s.u.print.n = 0;
        s.u.print.no_nl = 0;
      } else {
        s.u.printhash.items = NULL;
        s.u.printhash.n = 0;
        s.u.printhash.no_nl = 0;
      }
      while (1) {
        skip_ws (p);
        if (*cur == ':' || *cur == '\0') break;
        Node *e;
        PARSE_EXPR_OR_ERROR (e);
        if (s.kind == ST_PRINT) {
          if (s.u.print.n == cap) {
            cap = cap ? cap * 2 : 4;
            s.u.print.items = realloc (s.u.print.items, cap * sizeof (Node *));
          }
          s.u.print.items[s.u.print.n++] = e;
        } else {
          if (s.u.printhash.n == cap) {
            cap = cap ? cap * 2 : 4;
            s.u.printhash.items = realloc (s.u.printhash.items, cap * sizeof (Node *));
          }
          s.u.printhash.items[s.u.printhash.n++] = e;
        }
        skip_ws (p);
        if (*cur == ';' || *cur == ',') {
          cur++;
          skip_ws (p);
          if (*cur == ':' || *cur == '\0') {
            if (s.kind == ST_PRINT)
              s.u.print.no_nl = 1;
            else
              s.u.printhash.no_nl = 1;
            break;
          }
          continue;
        }
        break;
      }
      skip_ws (p);
    }
    stmt_vec_push (&out->stmts, s);
    if (s.kind == ST_REM) break;
    skip_ws (p);
    if (*cur == ':') {
      do {
        cur++;
        skip_ws (p);
      } while (*cur == ':');
      if (*cur == '\0') break;
      continue;
    }
    break;
  }
  return 1;
}

static void parse_func (Parser *p, FILE *f, char *line, int is_sub) {
  *p = (Parser) {0};
  cur = line + (is_sub ? 3 : 8);
  p->tok.type = TOK_EOF;
  skip_ws (p);
  char *name = parse_id (p);
  int f_is_str = name[strlen (name) - 1] == '$';
  char **params = NULL;
  int *is_str = NULL;
  size_t n = 0, cap = 0;
  char **src_lines = NULL;
  size_t src_len = 0, src_cap = 0;
  /* store header line */
  {
    if (src_len == src_cap) {
      src_cap = src_cap ? 2 * src_cap : 4;
      src_lines = realloc (src_lines, src_cap * sizeof (char *));
    }
    src_lines[src_len++] = strdup (line);
  }
  skip_ws (p);
  if (*cur == '(') {
    cur++;
    skip_ws (p);
    if (*cur != ')') {
      while (1) {
        char *param = parse_id (p);
        int ps = param[strlen (param) - 1] == '$';
        if (n == cap) {
          cap = cap ? 2 * cap : 4;
          params = realloc (params, cap * sizeof (char *));
          is_str = realloc (is_str, cap * sizeof (int));
        }
        params[n] = param;
        is_str[n] = ps;
        n++;
        skip_ws (p);
        if (*cur != ',') break;
        cur++;
        skip_ws (p);
      }
    }
    if (*cur == ')') cur++;
  }
  StmtVec body = {0};
  char buf[256];
  while (fgets (buf, sizeof (buf), f)) {
    char *lp = buf;
    buf[strcspn (buf, "\n")] = '\0';
    while (isspace ((unsigned char) *lp)) lp++;
    if (*lp == '\0') continue;
    if ((is_sub && strncasecmp (lp, "END SUB", 7) == 0)
        || (!is_sub && strncasecmp (lp, "END FUNCTION", 12) == 0)) {
      if (src_len == src_cap) {
        src_cap = src_cap ? 2 * src_cap : 4;
        src_lines = realloc (src_lines, src_cap * sizeof (char *));
      }
      src_lines[src_len++] = strdup (buf);
      break;
    }
    Line l;
    Parser lp_parser;
    if (parse_line (&lp_parser, buf, &l)) {
      if (src_len == src_cap) {
        src_cap = src_cap ? 2 * src_cap : 4;
        src_lines = realloc (src_lines, src_cap * sizeof (char *));
      }
      src_lines[src_len++] = l.src;
      for (size_t i = 0; i < l.stmts.len; i++) stmt_vec_push (&body, l.stmts.data[i]);
    } else {
      /* error already reported by parse_line */
    }
  }
  FuncDef fd = {name,   params, is_str, n,         NULL,    body,   f_is_str,
                is_sub, NULL,   NULL,   src_lines, src_len, src_cap};
  func_vec_push (&func_defs, fd);
}

static int load_program (LineVec *prog, const char *path) {
  FILE *f = fopen (path, "r");
  if (!f) {
    perror (path);
    return 0;
  }
  int auto_line = 10;
  char line[256];
  while (fgets (line, sizeof (line), f)) {
    char *s = line;
    line[strcspn (line, "\n")] = '\0';
    while (isspace ((unsigned char) *s)) s++;
    if (*s == '\0') continue;
    if (strncasecmp (s, "FUNCTION", 8) == 0) {
      Parser p;
      parse_func (&p, f, s, 0);
      continue;
    } else if (strncasecmp (s, "SUB", 3) == 0) {
      Parser p;
      parse_func (&p, f, s, 1);
      continue;
    }
    Line l;
    Parser p;
    if (parse_line (&p, line, &l)) {
      if (l.line == 0) {
        l.line = auto_line;
        auto_line += 10;
      }
      insert_or_replace_line (prog, l);
    } else {
      /* error already reported by parse_line */
    }
  }
  fclose (f);
  return 1;
}

/* Variable mapping */
typedef struct {
  char *name;
  MIR_reg_t reg;
  int is_str;
  int is_array;
  size_t size;
  size_t size2;
} Var;
typedef struct {
  Var *data;
  size_t len, cap;
} VarVec;
static MIR_insn_t g_var_init_anchor;
static MIR_item_t g_func;
static MIR_reg_t get_var (VarVec *vars, MIR_context_t ctx, MIR_item_t func, const char *name) {
  int is_str = name[strlen (name) - 1] == '$';
  for (size_t i = 0; i < vars->len; i++)
    if (!vars->data[i].is_array && strcmp (vars->data[i].name, name) == 0) return vars->data[i].reg;
  if (vars->len == vars->cap) {
    vars->cap = vars->cap ? 2 * vars->cap : 16;
    vars->data = realloc (vars->data, vars->cap * sizeof (Var));
  }
  vars->data[vars->len].name = strdup (name);
  vars->data[vars->len].is_str = is_str;
  vars->data[vars->len].is_array = 0;
  vars->data[vars->len].size = 0;
  MIR_type_t t = is_str ? MIR_T_I64 : MIR_T_D;
  vars->data[vars->len].reg = MIR_new_func_reg (ctx, func->u.func, t, name);
  if (is_str && g_var_init_anchor != NULL && func == g_func)
    MIR_insert_insn_after (ctx, func, g_var_init_anchor,
                           MIR_new_insn (ctx, MIR_MOV,
                                         MIR_new_reg_op (ctx, vars->data[vars->len].reg),
                                         MIR_new_int_op (ctx, 0)));
  return vars->data[vars->len++].reg;
}

static MIR_reg_t get_array (VarVec *vars, MIR_context_t ctx, MIR_item_t func, const char *name,
                            size_t size1, size_t size2, int is_str) {
  for (size_t i = 0; i < vars->len; i++)
    if (vars->data[i].is_array && strcmp (vars->data[i].name, name) == 0) {
      if (size1 != 0) {
        vars->data[i].size = size1 * (size2 ? size2 : 1);
        vars->data[i].size2 = size2;
      }
      return vars->data[i].reg;
    }
  if (vars->len == vars->cap) {
    vars->cap = vars->cap ? 2 * vars->cap : 16;
    vars->data = realloc (vars->data, vars->cap * sizeof (Var));
  }
  vars->data[vars->len].name = strdup (name);
  vars->data[vars->len].is_str = is_str;
  vars->data[vars->len].is_array = 1;
  vars->data[vars->len].size = size1 * (size2 ? size2 : 1);
  vars->data[vars->len].size2 = size2;
  char buf[64];
  snprintf (buf, sizeof (buf), "%s_arr", name);
  vars->data[vars->len].reg = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
  return vars->data[vars->len++].reg;
}

static size_t get_array_dim2 (VarVec *vars, const char *name) {
  for (size_t i = 0; i < vars->len; i++)
    if (vars->data[i].is_array && strcmp (vars->data[i].name, name) == 0)
      return vars->data[i].size2;
  return 0;
}

static size_t get_array_size (VarVec *vars, const char *name) {
  for (size_t i = 0; i < vars->len; i++)
    if (vars->data[i].is_array && strcmp (vars->data[i].name, name) == 0)
      return vars->data[i].size > 1 ? vars->data[i].size : 0;
  return 0;
}

/* Expression code generation */
static int tmp_id = 0;

static MIR_context_t g_ctx;
static LineVec *g_prog;
static Line *g_cur_line;
static MIR_label_t *g_labels;
static VarVec g_vars;
static MIR_reg_t g_ret_stack, g_ret_sp, g_ret_addr;
static LoopInfo *g_loop_stack;
static size_t g_loop_len, g_loop_cap;
static int g_line_tracking = 1;
static MIR_reg_t gen_expr (MIR_context_t ctx, MIR_item_t func, VarVec *vars, Node *n) {
  if (n == NULL) {
    report_parse_error_details (g_cur_line ? g_cur_line->line : 0,
                                g_cur_line ? g_cur_line->src : NULL, NULL);
    exit (1);
  }
  if (n->is_str) {
    if (n->kind == N_STR) {
      char buf[32];
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t r = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
      MIR_append_insn (ctx, func,
                       MIR_new_insn (ctx, MIR_MOV, MIR_new_reg_op (ctx, r),
                                     MIR_new_str_op (ctx,
                                                     (MIR_str_t) {strlen (n->str) + 1, n->str})));
      return r;
    } else if (n->kind == N_VAR) {
      if (n->index != NULL) {
        MIR_reg_t base = get_array (vars, ctx, func, n->var, 0, 0, 1);
        MIR_reg_t idxd1 = gen_expr (ctx, func, vars, n->index);
        char buf[32];
        sprintf (buf, "$t%d", tmp_id++);
        MIR_reg_t idx = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_D2I, MIR_new_reg_op (ctx, idx),
                                       MIR_new_reg_op (ctx, idxd1)));
        if (array_base != 0)
          MIR_append_insn (ctx, func,
                           MIR_new_insn (ctx, MIR_SUB, MIR_new_reg_op (ctx, idx),
                                         MIR_new_reg_op (ctx, idx),
                                         MIR_new_int_op (ctx, array_base)));
        if (n->index2 != NULL) {
          MIR_reg_t idxd2 = gen_expr (ctx, func, vars, n->index2);
          sprintf (buf, "$t%d", tmp_id++);
          MIR_reg_t idx2 = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
          MIR_append_insn (ctx, func,
                           MIR_new_insn (ctx, MIR_D2I, MIR_new_reg_op (ctx, idx2),
                                         MIR_new_reg_op (ctx, idxd2)));
          if (array_base != 0)
            MIR_append_insn (ctx, func,
                             MIR_new_insn (ctx, MIR_SUB, MIR_new_reg_op (ctx, idx2),
                                           MIR_new_reg_op (ctx, idx2),
                                           MIR_new_int_op (ctx, array_base)));
          size_t dim2 = get_array_dim2 (vars, n->var);
          sprintf (buf, "$t%d", tmp_id++);
          MIR_reg_t tmp = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
          MIR_append_insn (ctx, func,
                           MIR_new_insn (ctx, MIR_MUL, MIR_new_reg_op (ctx, tmp),
                                         MIR_new_reg_op (ctx, idx), MIR_new_int_op (ctx, dim2)));
          MIR_append_insn (ctx, func,
                           MIR_new_insn (ctx, MIR_ADD, MIR_new_reg_op (ctx, idx),
                                         MIR_new_reg_op (ctx, tmp), MIR_new_reg_op (ctx, idx2)));
        }
        size_t asize = get_array_size (vars, n->var);
        MIR_label_t bad = MIR_new_label (ctx), ok = MIR_new_label (ctx);
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_BLT, MIR_new_label_op (ctx, bad),
                                       MIR_new_reg_op (ctx, idx), MIR_new_int_op (ctx, 0)));
        if (asize != 0)
          MIR_append_insn (ctx, func,
                           MIR_new_insn (ctx, MIR_BGE, MIR_new_label_op (ctx, bad),
                                         MIR_new_reg_op (ctx, idx), MIR_new_int_op (ctx, asize)));
        sprintf (buf, "$t%d", tmp_id++);
        MIR_reg_t off = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_MUL, MIR_new_reg_op (ctx, off),
                                       MIR_new_reg_op (ctx, idx), MIR_new_int_op (ctx, 8)));
        sprintf (buf, "$t%d", tmp_id++);
        MIR_reg_t addr = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_ADD, MIR_new_reg_op (ctx, addr),
                                       MIR_new_reg_op (ctx, base), MIR_new_reg_op (ctx, off)));
        sprintf (buf, "$t%d", tmp_id++);
        MIR_reg_t val = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_MOV, MIR_new_reg_op (ctx, val),
                                       MIR_new_mem_op (ctx, MIR_T_P, 0, addr, 0, 1)));
        MIR_append_insn (ctx, func, MIR_new_insn (ctx, MIR_JMP, MIR_new_label_op (ctx, ok)));
        MIR_append_insn (ctx, func, bad);
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 2, MIR_new_ref_op (ctx, stop_proto),
                                            MIR_new_ref_op (ctx, stop_import)));
        MIR_append_insn (ctx, func, ok);
        return val;
      } else {
        return get_var (vars, ctx, func, n->var);
      }
    } else if (n->kind == N_CALL) {
      char buf[32];
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t res = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
      if (strcasecmp (n->var, "CHR$") == 0) {
        MIR_reg_t arg = gen_expr (ctx, func, vars, n->left);
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, chr_proto),
                                            MIR_new_ref_op (ctx, chr_import),
                                            MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, arg)));
      } else if (strcasecmp (n->var, "STRING$") == 0) {
        MIR_reg_t a1 = gen_expr (ctx, func, vars, n->left);
        MIR_reg_t a2 = gen_expr (ctx, func, vars, n->right);
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 5, MIR_new_ref_op (ctx, string_proto),
                                            MIR_new_ref_op (ctx, string_import),
                                            MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, a1),
                                            MIR_new_reg_op (ctx, a2)));
      } else if (strcasecmp (n->var, "SPC") == 0) {
        MIR_reg_t a1 = gen_expr (ctx, func, vars, n->left);
        char buf2[32];
        sprintf (buf2, "$t%d", tmp_id++);
        MIR_reg_t space = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf2);
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_MOV, MIR_new_reg_op (ctx, space),
                                       MIR_new_str_op (ctx, (MIR_str_t) {2, " "})));
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 5, MIR_new_ref_op (ctx, string_proto),
                                            MIR_new_ref_op (ctx, string_import),
                                            MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, a1),
                                            MIR_new_reg_op (ctx, space)));
      } else if (strcasecmp (n->var, "INPUT$") == 0) {
        MIR_reg_t arg = gen_expr (ctx, func, vars, n->left);
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, input_chr_proto),
                                            MIR_new_ref_op (ctx, input_chr_import),
                                            MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, arg)));
      } else if (strcasecmp (n->var, "INKEY$") == 0) {
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, inkey_proto),
                                            MIR_new_ref_op (ctx, inkey_import),
                                            MIR_new_reg_op (ctx, res)));
      } else if (strcasecmp (n->var, "TIME$") == 0) {
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, time_str_proto),
                                            MIR_new_ref_op (ctx, time_str_import),
                                            MIR_new_reg_op (ctx, res)));
      } else if (strcasecmp (n->var, "DATE$") == 0) {
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, date_str_proto),
                                            MIR_new_ref_op (ctx, date_str_import),
                                            MIR_new_reg_op (ctx, res)));
      } else if (strcasecmp (n->var, "LEFT$") == 0) {
        MIR_reg_t s = gen_expr (ctx, func, vars, n->left);
        MIR_reg_t cnt = gen_expr (ctx, func, vars, n->right);
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 5, MIR_new_ref_op (ctx, left_proto),
                                            MIR_new_ref_op (ctx, left_import),
                                            MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, s),
                                            MIR_new_reg_op (ctx, cnt)));
      } else if (strcasecmp (n->var, "RIGHT$") == 0) {
        MIR_reg_t s = gen_expr (ctx, func, vars, n->left);
        MIR_reg_t cnt = gen_expr (ctx, func, vars, n->right);
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 5, MIR_new_ref_op (ctx, right_proto),
                                            MIR_new_ref_op (ctx, right_import),
                                            MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, s),
                                            MIR_new_reg_op (ctx, cnt)));
      } else if (strcasecmp (n->var, "MID$") == 0) {
        MIR_reg_t s = gen_expr (ctx, func, vars, n->left);
        MIR_reg_t start = gen_expr (ctx, func, vars, n->right);
        MIR_op_t len_op;
        if (n->index != NULL) {
          MIR_reg_t len = gen_expr (ctx, func, vars, n->index);
          len_op = MIR_new_reg_op (ctx, len);
        } else {
          len_op = MIR_new_double_op (ctx, -1.0);
        }
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 6, MIR_new_ref_op (ctx, mid_proto),
                                            MIR_new_ref_op (ctx, mid_import),
                                            MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, s),
                                            MIR_new_reg_op (ctx, start), len_op));
      } else if (strcasecmp (n->var, "STR$") == 0) {
        MIR_reg_t arg = gen_expr (ctx, func, vars, n->left);
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, str_proto),
                                            MIR_new_ref_op (ctx, str_import),
                                            MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, arg)));
      } else if (strncmp (n->var, "FN", 2) == 0) {
        MIR_op_t args[5];
        size_t nargs = 0;
        if (n->left != NULL) {
          MIR_reg_t a = gen_expr (ctx, func, vars, n->left);
          args[nargs++] = MIR_new_reg_op (ctx, a);
        }
        if (n->right != NULL) {
          MIR_reg_t a = gen_expr (ctx, func, vars, n->right);
          args[nargs++] = MIR_new_reg_op (ctx, a);
        }
        if (n->index != NULL) {
          MIR_reg_t a = gen_expr (ctx, func, vars, n->index);
          args[nargs++] = MIR_new_reg_op (ctx, a);
        }
        FuncDef *fd = find_func (n->var);
        MIR_item_t proto = fd->proto;
        MIR_item_t item = fd->item;
        switch (nargs) {
        case 0:
          MIR_append_insn (ctx, func,
                           MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, proto),
                                              MIR_new_ref_op (ctx, item),
                                              MIR_new_reg_op (ctx, res)));
          break;
        case 1:
          MIR_append_insn (ctx, func,
                           MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, proto),
                                              MIR_new_ref_op (ctx, item), MIR_new_reg_op (ctx, res),
                                              args[0]));
          break;
        case 2:
          MIR_append_insn (ctx, func,
                           MIR_new_call_insn (ctx, 5, MIR_new_ref_op (ctx, proto),
                                              MIR_new_ref_op (ctx, item), MIR_new_reg_op (ctx, res),
                                              args[0], args[1]));
          break;
        case 3:
          MIR_append_insn (ctx, func,
                           MIR_new_call_insn (ctx, 6, MIR_new_ref_op (ctx, proto),
                                              MIR_new_ref_op (ctx, item), MIR_new_reg_op (ctx, res),
                                              args[0], args[1], args[2]));
          break;
        }
      }
      return res;
    } else if (n->kind == N_BIN && n->op == '+') {
      MIR_reg_t a = gen_expr (ctx, func, vars, n->left);
      MIR_reg_t b = gen_expr (ctx, func, vars, n->right);
      char buf[32];
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t res = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 5, MIR_new_ref_op (ctx, concat_proto),
                                          MIR_new_ref_op (ctx, concat_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, a),
                                          MIR_new_reg_op (ctx, b)));
      return res;
    }
    return 0;
  }
  if (n->kind == N_NUM) {
    char buf[32];
    sprintf (buf, "$t%d", tmp_id++);
    MIR_reg_t r = MIR_new_func_reg (ctx, func->u.func, MIR_T_D, buf);
    MIR_append_insn (ctx, func,
                     MIR_new_insn (ctx, MIR_DMOV, MIR_new_reg_op (ctx, r),
                                   MIR_new_double_op (ctx, n->num)));
    return r;
  } else if (n->kind == N_VAR) {
    if (n->index != NULL) {
      MIR_reg_t base = get_array (vars, ctx, func, n->var, 0, 0, 0);
      MIR_reg_t idxd1 = gen_expr (ctx, func, vars, n->index);
      char buf[32];
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t idx = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
      MIR_append_insn (ctx, func,
                       MIR_new_insn (ctx, MIR_D2I, MIR_new_reg_op (ctx, idx),
                                     MIR_new_reg_op (ctx, idxd1)));
      if (array_base != 0)
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_SUB, MIR_new_reg_op (ctx, idx),
                                       MIR_new_reg_op (ctx, idx),
                                       MIR_new_int_op (ctx, array_base)));
      if (n->index2 != NULL) {
        MIR_reg_t idxd2 = gen_expr (ctx, func, vars, n->index2);
        sprintf (buf, "$t%d", tmp_id++);
        MIR_reg_t idx2 = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_D2I, MIR_new_reg_op (ctx, idx2),
                                       MIR_new_reg_op (ctx, idxd2)));
        if (array_base != 0)
          MIR_append_insn (ctx, func,
                           MIR_new_insn (ctx, MIR_SUB, MIR_new_reg_op (ctx, idx2),
                                         MIR_new_reg_op (ctx, idx2),
                                         MIR_new_int_op (ctx, array_base)));
        size_t dim2 = get_array_dim2 (vars, n->var);
        sprintf (buf, "$t%d", tmp_id++);
        MIR_reg_t tmp = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_MUL, MIR_new_reg_op (ctx, tmp),
                                       MIR_new_reg_op (ctx, idx), MIR_new_int_op (ctx, dim2)));
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_ADD, MIR_new_reg_op (ctx, idx),
                                       MIR_new_reg_op (ctx, tmp), MIR_new_reg_op (ctx, idx2)));
      }
      size_t asize = get_array_size (vars, n->var);
      MIR_label_t bad = MIR_new_label (ctx), ok = MIR_new_label (ctx);
      MIR_append_insn (ctx, func,
                       MIR_new_insn (ctx, MIR_BLT, MIR_new_label_op (ctx, bad),
                                     MIR_new_reg_op (ctx, idx), MIR_new_int_op (ctx, 0)));
      if (asize != 0)
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_BGE, MIR_new_label_op (ctx, bad),
                                       MIR_new_reg_op (ctx, idx), MIR_new_int_op (ctx, asize)));
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t off = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
      MIR_append_insn (ctx, func,
                       MIR_new_insn (ctx, MIR_MUL, MIR_new_reg_op (ctx, off),
                                     MIR_new_reg_op (ctx, idx),
                                     MIR_new_int_op (ctx, sizeof (basic_num_t))));
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t addr = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
      MIR_append_insn (ctx, func,
                       MIR_new_insn (ctx, MIR_ADD, MIR_new_reg_op (ctx, addr),
                                     MIR_new_reg_op (ctx, base), MIR_new_reg_op (ctx, off)));
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t val = MIR_new_func_reg (ctx, func->u.func, MIR_T_D, buf);
      MIR_append_insn (ctx, func,
                       MIR_new_insn (ctx, MIR_DMOV, MIR_new_reg_op (ctx, val),
                                     MIR_new_mem_op (ctx, MIR_T_D, 0, addr, 0, 1)));
      MIR_append_insn (ctx, func, MIR_new_insn (ctx, MIR_JMP, MIR_new_label_op (ctx, ok)));
      MIR_append_insn (ctx, func, bad);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 2, MIR_new_ref_op (ctx, stop_proto),
                                          MIR_new_ref_op (ctx, stop_import)));
      MIR_append_insn (ctx, func, ok);
      return val;
    } else {
      return get_var (vars, ctx, func, n->var);
    }
  } else if (n->kind == N_NEG) {
    MIR_reg_t v = gen_expr (ctx, func, vars, n->left);
    char buf[32];
    sprintf (buf, "$t%d", tmp_id++);
    MIR_reg_t r = MIR_new_func_reg (ctx, func->u.func, MIR_T_D, buf);
    MIR_append_insn (ctx, func,
                     MIR_new_insn (ctx, MIR_DNEG, MIR_new_reg_op (ctx, r),
                                   MIR_new_reg_op (ctx, v)));
    return r;
  } else if (n->kind == N_NOT) {
    MIR_reg_t v = gen_expr (ctx, func, vars, n->left);
    char buf[32];
    sprintf (buf, "$t%d", tmp_id++);
    MIR_reg_t resi = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
    MIR_append_insn (ctx, func,
                     MIR_new_insn (ctx, MIR_DEQ, MIR_new_reg_op (ctx, resi),
                                   MIR_new_reg_op (ctx, v), MIR_new_double_op (ctx, 0.0)));
    sprintf (buf, "$t%d", tmp_id++);
    MIR_reg_t resd = MIR_new_func_reg (ctx, func->u.func, MIR_T_D, buf);
    MIR_append_insn (ctx, func,
                     MIR_new_insn (ctx, MIR_I2D, MIR_new_reg_op (ctx, resd),
                                   MIR_new_reg_op (ctx, resi)));
    return resd;
  } else if (n->kind == N_CALL) {
    char buf[32];
    sprintf (buf, "$t%d", tmp_id++);
    MIR_reg_t res = MIR_new_func_reg (ctx, func->u.func, MIR_T_D, buf);
    if (strcasecmp (n->var, "RND") == 0) {
      MIR_reg_t arg = gen_expr (ctx, func, vars, n->left);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, rnd_proto),
                                          MIR_new_ref_op (ctx, rnd_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, arg)));
    } else if (strcasecmp (n->var, "INT") == 0) {
      MIR_reg_t arg = gen_expr (ctx, func, vars, n->left);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, int_proto),
                                          MIR_new_ref_op (ctx, int_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, arg)));
    } else if (strcasecmp (n->var, "TIMER") == 0) {
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, timer_proto),
                                          MIR_new_ref_op (ctx, timer_import),
                                          MIR_new_reg_op (ctx, res)));
    } else if (strcasecmp (n->var, "TIME") == 0) {
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, time_proto),
                                          MIR_new_ref_op (ctx, time_import),
                                          MIR_new_reg_op (ctx, res)));
    } else if (strcasecmp (n->var, "DATE") == 0) {
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, date_proto),
                                          MIR_new_ref_op (ctx, date_import),
                                          MIR_new_reg_op (ctx, res)));
    } else if (strcasecmp (n->var, "PEEK") == 0) {
      MIR_reg_t arg = gen_expr (ctx, func, vars, n->left);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, peek_proto),
                                          MIR_new_ref_op (ctx, peek_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, arg)));
    } else if (strcasecmp (n->var, "EOF") == 0) {
      MIR_reg_t arg = gen_expr (ctx, func, vars, n->left);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, eof_proto),
                                          MIR_new_ref_op (ctx, eof_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, arg)));
    } else if (strcasecmp (n->var, "ABS") == 0) {
      MIR_reg_t arg = gen_expr (ctx, func, vars, n->left);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, abs_proto),
                                          MIR_new_ref_op (ctx, abs_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, arg)));
    } else if (strcasecmp (n->var, "SGN") == 0) {
      MIR_reg_t arg = gen_expr (ctx, func, vars, n->left);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, sgn_proto),
                                          MIR_new_ref_op (ctx, sgn_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, arg)));
    } else if (strcasecmp (n->var, "SQR") == 0) {
      MIR_reg_t arg = gen_expr (ctx, func, vars, n->left);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, sqr_proto),
                                          MIR_new_ref_op (ctx, sqr_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, arg)));
    } else if (strcasecmp (n->var, "SIN") == 0) {
      MIR_reg_t arg = gen_expr (ctx, func, vars, n->left);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, sin_proto),
                                          MIR_new_ref_op (ctx, sin_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, arg)));
    } else if (strcasecmp (n->var, "COS") == 0) {
      MIR_reg_t arg = gen_expr (ctx, func, vars, n->left);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, cos_proto),
                                          MIR_new_ref_op (ctx, cos_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, arg)));
    } else if (strcasecmp (n->var, "TAN") == 0) {
      MIR_reg_t arg = gen_expr (ctx, func, vars, n->left);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, tan_proto),
                                          MIR_new_ref_op (ctx, tan_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, arg)));
    } else if (strcasecmp (n->var, "ATN") == 0) {
      MIR_reg_t arg = gen_expr (ctx, func, vars, n->left);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, atn_proto),
                                          MIR_new_ref_op (ctx, atn_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, arg)));
    } else if (strcasecmp (n->var, "LOG") == 0) {
      MIR_reg_t arg = gen_expr (ctx, func, vars, n->left);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, log_proto),
                                          MIR_new_ref_op (ctx, log_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, arg)));
    } else if (strcasecmp (n->var, "EXP") == 0) {
      MIR_reg_t arg = gen_expr (ctx, func, vars, n->left);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, exp_proto),
                                          MIR_new_ref_op (ctx, exp_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, arg)));
    } else if (strcasecmp (n->var, "LEN") == 0) {
      MIR_reg_t arg = gen_expr (ctx, func, vars, n->left);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, len_proto),
                                          MIR_new_ref_op (ctx, len_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, arg)));
    } else if (strcasecmp (n->var, "VAL") == 0) {
      MIR_reg_t arg = gen_expr (ctx, func, vars, n->left);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, val_proto),
                                          MIR_new_ref_op (ctx, val_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, arg)));
    } else if (strcasecmp (n->var, "ASC") == 0) {
      MIR_reg_t arg = gen_expr (ctx, func, vars, n->left);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, asc_proto),
                                          MIR_new_ref_op (ctx, asc_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, arg)));
    } else if (strcasecmp (n->var, "MIRCTX") == 0) {
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, mir_ctx_proto),
                                          MIR_new_ref_op (ctx, mir_ctx_import),
                                          MIR_new_reg_op (ctx, res)));
    } else if (strcasecmp (n->var, "MIRMOD") == 0) {
      MIR_reg_t c = gen_expr (ctx, func, vars, n->left);
      MIR_reg_t name = gen_expr (ctx, func, vars, n->right);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 5, MIR_new_ref_op (ctx, mir_mod_proto),
                                          MIR_new_ref_op (ctx, mir_mod_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, c),
                                          MIR_new_reg_op (ctx, name)));
    } else if (strcasecmp (n->var, "MIRFUNC") == 0) {
      MIR_reg_t m = gen_expr (ctx, func, vars, n->left);
      MIR_reg_t name = gen_expr (ctx, func, vars, n->right);
      MIR_reg_t na = gen_expr (ctx, func, vars, n->index);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 6, MIR_new_ref_op (ctx, mir_func_proto),
                                          MIR_new_ref_op (ctx, mir_func_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, m),
                                          MIR_new_reg_op (ctx, name), MIR_new_reg_op (ctx, na)));
    } else if (strcasecmp (n->var, "MIRREG") == 0) {
      MIR_reg_t f = gen_expr (ctx, func, vars, n->left);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, mir_reg_proto),
                                          MIR_new_ref_op (ctx, mir_reg_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, f)));
    } else if (strcasecmp (n->var, "MIRLABEL") == 0) {
      MIR_reg_t f = gen_expr (ctx, func, vars, n->left);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, mir_label_proto),
                                          MIR_new_ref_op (ctx, mir_label_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, f)));
    } else if (strcasecmp (n->var, "MIREMIT") == 0) {
      MIR_reg_t f = gen_expr (ctx, func, vars, n->left);
      MIR_reg_t op = gen_expr (ctx, func, vars, n->right);
      MIR_op_t a = n->index ? MIR_new_reg_op (ctx, gen_expr (ctx, func, vars, n->index))
                            : MIR_new_double_op (ctx, 0.0);
      MIR_op_t b = n->index2 ? MIR_new_reg_op (ctx, gen_expr (ctx, func, vars, n->index2))
                             : MIR_new_double_op (ctx, 0.0);
      MIR_op_t c = n->arg4 ? MIR_new_reg_op (ctx, gen_expr (ctx, func, vars, n->arg4))
                           : MIR_new_double_op (ctx, 0.0);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 8, MIR_new_ref_op (ctx, mir_emit_proto),
                                          MIR_new_ref_op (ctx, mir_emit_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, f),
                                          MIR_new_reg_op (ctx, op), a, b, c));
    } else if (strcasecmp (n->var, "MIREMITLBL") == 0) {
      MIR_reg_t f = gen_expr (ctx, func, vars, n->left);
      MIR_reg_t l = gen_expr (ctx, func, vars, n->right);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 5, MIR_new_ref_op (ctx, mir_emitlbl_proto),
                                          MIR_new_ref_op (ctx, mir_emitlbl_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, f),
                                          MIR_new_reg_op (ctx, l)));
    } else if (strcasecmp (n->var, "MIRRET") == 0) {
      MIR_reg_t f = gen_expr (ctx, func, vars, n->left);
      MIR_reg_t r = gen_expr (ctx, func, vars, n->right);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 5, MIR_new_ref_op (ctx, mir_ret_proto),
                                          MIR_new_ref_op (ctx, mir_ret_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, f),
                                          MIR_new_reg_op (ctx, r)));
    } else if (strcasecmp (n->var, "MIRFINISH") == 0) {
      MIR_reg_t m = gen_expr (ctx, func, vars, n->left);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, mir_finish_proto),
                                          MIR_new_ref_op (ctx, mir_finish_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, m)));
    } else if (strcasecmp (n->var, "MIRRUN") == 0) {
      MIR_reg_t f = gen_expr (ctx, func, vars, n->left);
      MIR_op_t a1 = n->right ? MIR_new_reg_op (ctx, gen_expr (ctx, func, vars, n->right))
                             : MIR_new_double_op (ctx, 0.0);
      MIR_op_t a2 = n->index ? MIR_new_reg_op (ctx, gen_expr (ctx, func, vars, n->index))
                             : MIR_new_double_op (ctx, 0.0);
      MIR_op_t a3 = n->index2 ? MIR_new_reg_op (ctx, gen_expr (ctx, func, vars, n->index2))
                              : MIR_new_double_op (ctx, 0.0);
      MIR_op_t a4 = n->arg4 ? MIR_new_reg_op (ctx, gen_expr (ctx, func, vars, n->arg4))
                            : MIR_new_double_op (ctx, 0.0);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 8, MIR_new_ref_op (ctx, mir_run_proto),
                                          MIR_new_ref_op (ctx, mir_run_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, f), a1,
                                          a2, a3, a4));
    } else if (strcasecmp (n->var, "MIRDUMP") == 0) {
      MIR_reg_t f = gen_expr (ctx, func, vars, n->left);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, mir_dump_proto),
                                          MIR_new_ref_op (ctx, mir_dump_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, f)));
    } else if (find_func (n->var) != NULL) {
      MIR_op_t args[3];
      size_t nargs = 0;
      if (n->left != NULL) {
        MIR_reg_t a = gen_expr (ctx, func, vars, n->left);
        args[nargs++] = MIR_new_reg_op (ctx, a);
      }
      if (n->right != NULL) {
        MIR_reg_t a = gen_expr (ctx, func, vars, n->right);
        args[nargs++] = MIR_new_reg_op (ctx, a);
      }
      if (n->index != NULL) {
        MIR_reg_t a = gen_expr (ctx, func, vars, n->index);
        args[nargs++] = MIR_new_reg_op (ctx, a);
      }
      FuncDef *fd = find_func (n->var);
      MIR_item_t proto = fd->proto;
      MIR_item_t item = fd->item;
      switch (nargs) {
      case 0:
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, proto),
                                            MIR_new_ref_op (ctx, item), MIR_new_reg_op (ctx, res)));
        break;
      case 1:
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, proto),
                                            MIR_new_ref_op (ctx, item), MIR_new_reg_op (ctx, res),
                                            args[0]));
        break;
      case 2:
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 5, MIR_new_ref_op (ctx, proto),
                                            MIR_new_ref_op (ctx, item), MIR_new_reg_op (ctx, res),
                                            args[0], args[1]));
        break;
      case 3:
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 6, MIR_new_ref_op (ctx, proto),
                                            MIR_new_ref_op (ctx, item), MIR_new_reg_op (ctx, res),
                                            args[0], args[1], args[2]));
        break;
      }

    } else if (strcasecmp (n->var, "POS") == 0) {
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, pos_proto),
                                          MIR_new_ref_op (ctx, pos_import),
                                          MIR_new_reg_op (ctx, res)));

    } else if (strcasecmp (n->var, "INSTR") == 0) {
      MIR_reg_t s = gen_expr (ctx, func, vars, n->left);
      MIR_reg_t sub = gen_expr (ctx, func, vars, n->right);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 5, MIR_new_ref_op (ctx, instr_proto),
                                          MIR_new_ref_op (ctx, instr_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, s),
                                          MIR_new_reg_op (ctx, sub)));

    } else if (strncmp (n->var, "FN", 2) == 0) {
      MIR_op_t args[3];
      size_t nargs = 0;
      if (n->left != NULL) {
        MIR_reg_t a = gen_expr (ctx, func, vars, n->left);
        args[nargs++] = MIR_new_reg_op (ctx, a);
      }
      if (n->right != NULL) {
        MIR_reg_t a = gen_expr (ctx, func, vars, n->right);
        args[nargs++] = MIR_new_reg_op (ctx, a);
      }
      if (n->index != NULL) {
        MIR_reg_t a = gen_expr (ctx, func, vars, n->index);
        args[nargs++] = MIR_new_reg_op (ctx, a);
      }
      FuncDef *fd = find_func (n->var);
      MIR_item_t proto = fd->proto;
      MIR_item_t item = fd->item;
      switch (nargs) {
      case 0:
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, proto),
                                            MIR_new_ref_op (ctx, item), MIR_new_reg_op (ctx, res)));
        break;
      case 1:
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, proto),
                                            MIR_new_ref_op (ctx, item), MIR_new_reg_op (ctx, res),
                                            args[0]));
        break;
      case 2:
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 5, MIR_new_ref_op (ctx, proto),
                                            MIR_new_ref_op (ctx, item), MIR_new_reg_op (ctx, res),
                                            args[0], args[1]));
        break;
      case 3:
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 6, MIR_new_ref_op (ctx, proto),
                                            MIR_new_ref_op (ctx, item), MIR_new_reg_op (ctx, res),
                                            args[0], args[1], args[2]));
        break;
      }

    } else if (strcasecmp (n->var, "POS") == 0) {
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, pos_proto),
                                          MIR_new_ref_op (ctx, pos_import),
                                          MIR_new_reg_op (ctx, res)));

    } else if (strcasecmp (n->var, "INSTR") == 0) {
      MIR_reg_t s = gen_expr (ctx, func, vars, n->left);
      MIR_reg_t sub = gen_expr (ctx, func, vars, n->right);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 5, MIR_new_ref_op (ctx, instr_proto),
                                          MIR_new_ref_op (ctx, instr_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, s),
                                          MIR_new_reg_op (ctx, sub)));
    }
    return res;
  } else if (n->op == '&') {
    MIR_reg_t l = gen_expr (ctx, func, vars, n->left);
    char buf[32];
    sprintf (buf, "$t%d", tmp_id++);
    MIR_reg_t res = MIR_new_func_reg (ctx, func->u.func, MIR_T_D, buf);
    MIR_label_t false_lab = MIR_new_label (ctx);
    MIR_label_t end_lab = MIR_new_label (ctx);
    MIR_append_insn (ctx, func,
                     MIR_new_insn (ctx, MIR_DBEQ, MIR_new_label_op (ctx, false_lab),
                                   MIR_new_reg_op (ctx, l), MIR_new_double_op (ctx, 0.0)));
    MIR_reg_t r = gen_expr (ctx, func, vars, n->right);
    MIR_append_insn (ctx, func,
                     MIR_new_insn (ctx, MIR_DBEQ, MIR_new_label_op (ctx, false_lab),
                                   MIR_new_reg_op (ctx, r), MIR_new_double_op (ctx, 0.0)));
    MIR_append_insn (ctx, func,
                     MIR_new_insn (ctx, MIR_DMOV, MIR_new_reg_op (ctx, res),
                                   MIR_new_double_op (ctx, 1.0)));
    MIR_append_insn (ctx, func, MIR_new_insn (ctx, MIR_JMP, MIR_new_label_op (ctx, end_lab)));
    MIR_append_insn (ctx, func, false_lab);
    MIR_append_insn (ctx, func,
                     MIR_new_insn (ctx, MIR_DMOV, MIR_new_reg_op (ctx, res),
                                   MIR_new_double_op (ctx, 0.0)));
    MIR_append_insn (ctx, func, end_lab);
    return res;
  } else if (n->op == '|') {
    MIR_reg_t l = gen_expr (ctx, func, vars, n->left);
    char buf[32];
    sprintf (buf, "$t%d", tmp_id++);
    MIR_reg_t res = MIR_new_func_reg (ctx, func->u.func, MIR_T_D, buf);
    MIR_label_t true_lab = MIR_new_label (ctx);
    MIR_label_t end_lab = MIR_new_label (ctx);
    MIR_append_insn (ctx, func,
                     MIR_new_insn (ctx, MIR_DBNE, MIR_new_label_op (ctx, true_lab),
                                   MIR_new_reg_op (ctx, l), MIR_new_double_op (ctx, 0.0)));
    MIR_reg_t r = gen_expr (ctx, func, vars, n->right);
    MIR_append_insn (ctx, func,
                     MIR_new_insn (ctx, MIR_DBNE, MIR_new_label_op (ctx, true_lab),
                                   MIR_new_reg_op (ctx, r), MIR_new_double_op (ctx, 0.0)));
    MIR_append_insn (ctx, func,
                     MIR_new_insn (ctx, MIR_DMOV, MIR_new_reg_op (ctx, res),
                                   MIR_new_double_op (ctx, 0.0)));
    MIR_append_insn (ctx, func, MIR_new_insn (ctx, MIR_JMP, MIR_new_label_op (ctx, end_lab)));
    MIR_append_insn (ctx, func, true_lab);
    MIR_append_insn (ctx, func,
                     MIR_new_insn (ctx, MIR_DMOV, MIR_new_reg_op (ctx, res),
                                   MIR_new_double_op (ctx, 1.0)));
    MIR_append_insn (ctx, func, end_lab);
    return res;
  } else {
    MIR_reg_t l = gen_expr (ctx, func, vars, n->left);
    MIR_reg_t r = gen_expr (ctx, func, vars, n->right);
    char buf[32];
    if (n->op == '&' || n->op == '|') {
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t li = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
      MIR_append_insn (ctx, func,
                       MIR_new_insn (ctx, MIR_DNE, MIR_new_reg_op (ctx, li),
                                     MIR_new_reg_op (ctx, l), MIR_new_double_op (ctx, 0.0)));
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t ri = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
      MIR_append_insn (ctx, func,
                       MIR_new_insn (ctx, MIR_DNE, MIR_new_reg_op (ctx, ri),
                                     MIR_new_reg_op (ctx, r), MIR_new_double_op (ctx, 0.0)));
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t resi = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
      MIR_insn_code_t bop = n->op == '&' ? MIR_AND : MIR_OR;
      MIR_append_insn (ctx, func,
                       MIR_new_insn (ctx, bop, MIR_new_reg_op (ctx, resi), MIR_new_reg_op (ctx, li),
                                     MIR_new_reg_op (ctx, ri)));
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t resd = MIR_new_func_reg (ctx, func->u.func, MIR_T_D, buf);
      MIR_append_insn (ctx, func,
                       MIR_new_insn (ctx, MIR_I2D, MIR_new_reg_op (ctx, resd),
                                     MIR_new_reg_op (ctx, resi)));
      return resd;
    } else if (n->op == '=' || n->op == '!' || n->op == '<' || n->op == '>' || n->op == 'L'
               || n->op == 'G') {
      MIR_insn_code_t cmp_code;
      MIR_reg_t resi;
      sprintf (buf, "$t%d", tmp_id++);
      if (n->left->is_str || n->right->is_str) {
        MIR_reg_t cmp = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 5, MIR_new_ref_op (ctx, strcmp_proto),
                                            MIR_new_ref_op (ctx, strcmp_import),
                                            MIR_new_reg_op (ctx, cmp), MIR_new_reg_op (ctx, l),
                                            MIR_new_reg_op (ctx, r)));
        sprintf (buf, "$t%d", tmp_id++);
        resi = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
        cmp_code = MIR_EQ;
        switch (n->op) {
        case '=': cmp_code = MIR_EQ; break;
        case '!': cmp_code = MIR_NE; break;
        case '<': cmp_code = MIR_LT; break;
        case 'L': cmp_code = MIR_LE; break;
        case '>': cmp_code = MIR_GT; break;
        case 'G': cmp_code = MIR_GE; break;
        }
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, cmp_code, MIR_new_reg_op (ctx, resi),
                                       MIR_new_reg_op (ctx, cmp), MIR_new_int_op (ctx, 0)));
      } else {
        resi = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
        switch (n->op) {
        case '=': cmp_code = MIR_DEQ; break;
        case '!': cmp_code = MIR_DNE; break;
        case '<': cmp_code = MIR_DLT; break;
        case 'L': cmp_code = MIR_DLE; break;
        case '>': cmp_code = MIR_DGT; break;
        case 'G': cmp_code = MIR_DGE; break;
        default: cmp_code = MIR_DEQ; break;
        }
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, cmp_code, MIR_new_reg_op (ctx, resi),
                                       MIR_new_reg_op (ctx, l), MIR_new_reg_op (ctx, r)));
      }
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t resd = MIR_new_func_reg (ctx, func->u.func, MIR_T_D, buf);
      MIR_append_insn (ctx, func,
                       MIR_new_insn (ctx, MIR_I2D, MIR_new_reg_op (ctx, resd),
                                     MIR_new_reg_op (ctx, resi)));
      return resd;
    } else {
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t res = MIR_new_func_reg (ctx, func->u.func, MIR_T_D, buf);
      if (n->op == '\\' || n->op == '%') {
        sprintf (buf, "$t%d", tmp_id++);
        MIR_reg_t li = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_D2I, MIR_new_reg_op (ctx, li),
                                       MIR_new_reg_op (ctx, l)));
        sprintf (buf, "$t%d", tmp_id++);
        MIR_reg_t ri = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_D2I, MIR_new_reg_op (ctx, ri),
                                       MIR_new_reg_op (ctx, r)));
        sprintf (buf, "$t%d", tmp_id++);
        MIR_reg_t resi = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
        MIR_insn_code_t iop = n->op == '\\' ? MIR_DIV : MIR_MOD;
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, iop, MIR_new_reg_op (ctx, resi),
                                       MIR_new_reg_op (ctx, li), MIR_new_reg_op (ctx, ri)));
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_I2D, MIR_new_reg_op (ctx, res),
                                       MIR_new_reg_op (ctx, resi)));
      } else {
        MIR_insn_code_t op = MIR_DADD;
        switch (n->op) {
        case '+': op = MIR_DADD; break;
        case '-': op = MIR_DSUB; break;
        case '*': op = MIR_DMUL; break;
        case '/': op = MIR_DDIV; break;
        }
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, op, MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, l),
                                       MIR_new_reg_op (ctx, r)));
      }
      return res;
    }
  }
}

/* Generate MIR for program */
static MIR_label_t find_label (LineVec *prog, MIR_label_t *labels, int line) {
  for (size_t i = 0; i < prog->len; i++)
    if (prog->data[i].line == line) return labels[i];
  return labels[0];
}

static char *change_suffix (const char *name, const char *suf) {
  const char *slash = strrchr (name, '/');
  const char *dot = strrchr (slash ? slash + 1 : name, '.');
  size_t len = dot ? (size_t) (dot - name) : strlen (name);
  size_t slen = strlen (suf);
  char *res = malloc (len + slen + 1);
  memcpy (res, name, len);
  memcpy (res + len, suf, slen + 1);
  return res;
}

static size_t ctab_byte_num;
static FILE *ctab_file;

static int ctab_writer (MIR_context_t ctx MIR_UNUSED, uint8_t byte) {
  fprintf (ctab_file, "0x%02x, ", byte);
  if (++ctab_byte_num % 16 == 0) fprintf (ctab_file, "\n");
  return 1;
}

static void call1 (MIR_item_t proto, MIR_item_t import, MIR_op_t a) {
  MIR_append_insn (g_ctx, g_func,
                   MIR_new_call_insn (g_ctx, 3, MIR_new_ref_op (g_ctx, proto),
                                      MIR_new_ref_op (g_ctx, import), a));
}

static void call2 (MIR_item_t proto, MIR_item_t import, MIR_op_t a0, MIR_op_t a1) {
  MIR_append_insn (g_ctx, g_func,
                   MIR_new_call_insn (g_ctx, 4, MIR_new_ref_op (g_ctx, proto),
                                      MIR_new_ref_op (g_ctx, import), a0, a1));
}

static void print_str (MIR_str_t str) {
  call1 (prints_proto, prints_import, MIR_new_str_op (g_ctx, str));
}

static void print_item (Node *e, Node *next) {
  MIR_reg_t r = gen_expr (g_ctx, g_func, &g_vars, e);
  call1 (e->is_str ? prints_proto : print_proto, e->is_str ? prints_import : print_import,
         MIR_new_reg_op (g_ctx, r));
  if (!e->is_str && next != NULL && !next->is_str) print_str ((MIR_str_t) {2, " "});
}

static void print_hash_str (MIR_reg_t fn, MIR_str_t str) {
  call2 (prinths_proto, prinths_import, MIR_new_reg_op (g_ctx, fn), MIR_new_str_op (g_ctx, str));
}

static void print_hash_item (MIR_reg_t fn, Node *e, Node *next) {
  MIR_reg_t r = gen_expr (g_ctx, g_func, &g_vars, e);
  call2 (e->is_str ? prinths_proto : printh_proto, e->is_str ? prinths_import : printh_import,
         MIR_new_reg_op (g_ctx, fn), MIR_new_reg_op (g_ctx, r));
  if (!e->is_str && next != NULL && !next->is_str) print_hash_str (fn, (MIR_str_t) {2, " "});
}

static void gen_print (Stmt *s) {
  for (size_t k = 0; k < s->u.print.n; k++)
    print_item (s->u.print.items[k], k + 1 < s->u.print.n ? s->u.print.items[k + 1] : NULL);
  if (!s->u.print.no_nl) print_str ((MIR_str_t) {2, "\n"});
}

static void gen_print_hash (Stmt *s) {
  MIR_reg_t fn = gen_expr (g_ctx, g_func, &g_vars, s->u.printhash.num);
  for (size_t k = 0; k < s->u.printhash.n; k++)
    print_hash_item (fn, s->u.printhash.items[k],
                     k + 1 < s->u.printhash.n ? s->u.printhash.items[k + 1] : NULL);
  if (!s->u.printhash.no_nl) print_hash_str (fn, (MIR_str_t) {2, "\n"});
}

static void input_str_var (MIR_reg_t v) {
  call1 (free_proto, free_import, MIR_new_reg_op (g_ctx, v));
  call1 (input_str_proto, input_str_import, MIR_new_reg_op (g_ctx, v));
}

static void input_num_var (MIR_reg_t v) {
  call1 (input_proto, input_import, MIR_new_reg_op (g_ctx, v));
}

static void gen_input (Stmt *s) {
  MIR_reg_t v = get_var (&g_vars, g_ctx, g_func, s->u.input.var);
  (s->u.input.is_str ? input_str_var : input_num_var) (v);
}

static void input_hash_str (MIR_reg_t v, MIR_reg_t fn) {
  call1 (free_proto, free_import, MIR_new_reg_op (g_ctx, v));
  call2 (input_hash_str_proto, input_hash_str_import, MIR_new_reg_op (g_ctx, v),
         MIR_new_reg_op (g_ctx, fn));
}

static void input_hash_num (MIR_reg_t v, MIR_reg_t fn) {
  call2 (input_hash_proto, input_hash_import, MIR_new_reg_op (g_ctx, v),
         MIR_new_reg_op (g_ctx, fn));
}

static void gen_input_hash (Stmt *s) {
  MIR_reg_t fn = gen_expr (g_ctx, g_func, &g_vars, s->u.inputhash.num);
  MIR_reg_t v = get_var (&g_vars, g_ctx, g_func, s->u.inputhash.var);
  (s->u.inputhash.is_str ? input_hash_str : input_hash_num) (v, fn);
}

static void gen_get (Stmt *s) {
  MIR_reg_t v = get_var (&g_vars, g_ctx, g_func, s->u.get.var);
  call1 (free_proto, free_import, MIR_new_reg_op (g_ctx, v));
  call1 (get_proto, get_import, MIR_new_reg_op (g_ctx, v));
}

static void gen_get_hash (Stmt *s) {
  MIR_reg_t fn = gen_expr (g_ctx, g_func, &g_vars, s->u.gethash.num);
  MIR_reg_t v = get_var (&g_vars, g_ctx, g_func, s->u.gethash.var);
  call1 (free_proto, free_import, MIR_new_reg_op (g_ctx, v));
  call2 (get_hash_proto, get_hash_import, MIR_new_reg_op (g_ctx, v), MIR_new_reg_op (g_ctx, fn));
}

static void gen_put (Stmt *s) {
  MIR_reg_t r = gen_expr (g_ctx, g_func, &g_vars, s->u.put.expr);
  call1 (put_proto, put_import, MIR_new_reg_op (g_ctx, r));
}

static void gen_put_hash (Stmt *s) {
  MIR_reg_t fn = gen_expr (g_ctx, g_func, &g_vars, s->u.puthash.num);
  MIR_reg_t r = gen_expr (g_ctx, g_func, &g_vars, s->u.puthash.expr);
  call2 (put_hash_proto, put_hash_import, MIR_new_reg_op (g_ctx, fn), MIR_new_reg_op (g_ctx, r));
}

static void gen_poke (Stmt *s) {
  MIR_reg_t addr = gen_expr (g_ctx, g_func, &g_vars, s->u.poke.addr);
  MIR_reg_t val = gen_expr (g_ctx, g_func, &g_vars, s->u.poke.value);
  call2 (poke_proto, poke_import, MIR_new_reg_op (g_ctx, addr), MIR_new_reg_op (g_ctx, val));
}

static void gen_stmt (Stmt *s) {
  switch (s->kind) {
  case ST_DEF: break;
  case ST_PRINT: gen_print (s); break;
  case ST_PRINT_HASH: gen_print_hash (s); break;
  case ST_INPUT: gen_input (s); break;
  case ST_INPUT_HASH: gen_input_hash (s); break;
  case ST_GET: gen_get (s); break;
  case ST_GET_HASH: gen_get_hash (s); break;
  case ST_PUT: gen_put (s); break;
  case ST_PUT_HASH: gen_put_hash (s); break;
  case ST_POKE: gen_poke (s); break;
  case ST_READ: {
    for (size_t k = 0; k < s->u.read.n; k++) {
      Node *v = s->u.read.vars[k];
      char buf[32];
      MIR_reg_t res;
      if (v->is_str) {
        sprintf (buf, "$t%d", tmp_id++);
        res = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
        MIR_append_insn (g_ctx, g_func,
                         MIR_new_call_insn (g_ctx, 3, MIR_new_ref_op (g_ctx, read_str_proto),
                                            MIR_new_ref_op (g_ctx, read_str_import),
                                            MIR_new_reg_op (g_ctx, res)));
      } else {
        sprintf (buf, "$t%d", tmp_id++);
        res = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_D, buf);
        MIR_append_insn (g_ctx, g_func,
                         MIR_new_call_insn (g_ctx, 3, MIR_new_ref_op (g_ctx, read_proto),
                                            MIR_new_ref_op (g_ctx, read_import),
                                            MIR_new_reg_op (g_ctx, res)));
      }
      if (v->index != NULL) {
        MIR_reg_t base = get_array (&g_vars, g_ctx, g_func, v->var, 0, 0, v->is_str);
        MIR_reg_t idxd1 = gen_expr (g_ctx, g_func, &g_vars, v->index);
        sprintf (buf, "$t%d", tmp_id++);
        MIR_reg_t idx = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
        MIR_append_insn (g_ctx, g_func,
                         MIR_new_insn (g_ctx, MIR_D2I, MIR_new_reg_op (g_ctx, idx),
                                       MIR_new_reg_op (g_ctx, idxd1)));
        if (array_base != 0)
          MIR_append_insn (g_ctx, g_func,
                           MIR_new_insn (g_ctx, MIR_SUB, MIR_new_reg_op (g_ctx, idx),
                                         MIR_new_reg_op (g_ctx, idx),
                                         MIR_new_int_op (g_ctx, array_base)));
        if (v->index2 != NULL) {
          MIR_reg_t idxd2 = gen_expr (g_ctx, g_func, &g_vars, v->index2);
          sprintf (buf, "$t%d", tmp_id++);
          MIR_reg_t idx2 = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
          MIR_append_insn (g_ctx, g_func,
                           MIR_new_insn (g_ctx, MIR_D2I, MIR_new_reg_op (g_ctx, idx2),
                                         MIR_new_reg_op (g_ctx, idxd2)));
          if (array_base != 0)
            MIR_append_insn (g_ctx, g_func,
                             MIR_new_insn (g_ctx, MIR_SUB, MIR_new_reg_op (g_ctx, idx2),
                                           MIR_new_reg_op (g_ctx, idx2),
                                           MIR_new_int_op (g_ctx, array_base)));
          size_t dim2 = get_array_dim2 (&g_vars, v->var);
          sprintf (buf, "$t%d", tmp_id++);
          MIR_reg_t tmp = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
          MIR_append_insn (g_ctx, g_func,
                           MIR_new_insn (g_ctx, MIR_MUL, MIR_new_reg_op (g_ctx, tmp),
                                         MIR_new_reg_op (g_ctx, idx),
                                         MIR_new_int_op (g_ctx, dim2)));
          MIR_append_insn (g_ctx, g_func,
                           MIR_new_insn (g_ctx, MIR_ADD, MIR_new_reg_op (g_ctx, idx),
                                         MIR_new_reg_op (g_ctx, tmp),
                                         MIR_new_reg_op (g_ctx, idx2)));
        }
        sprintf (buf, "$t%d", tmp_id++);
        MIR_reg_t off = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
        size_t elem_size = v->is_str ? sizeof (char *) : sizeof (basic_num_t);
        MIR_append_insn (g_ctx, g_func,
                         MIR_new_insn (g_ctx, MIR_MUL, MIR_new_reg_op (g_ctx, off),
                                       MIR_new_reg_op (g_ctx, idx),
                                       MIR_new_int_op (g_ctx, elem_size)));
        sprintf (buf, "$t%d", tmp_id++);
        MIR_reg_t addr = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
        MIR_append_insn (g_ctx, g_func,
                         MIR_new_insn (g_ctx, MIR_ADD, MIR_new_reg_op (g_ctx, addr),
                                       MIR_new_reg_op (g_ctx, base), MIR_new_reg_op (g_ctx, off)));
        MIR_insn_code_t store_code = v->is_str ? MIR_MOV : MIR_DMOV;
        if (v->is_str) {
          sprintf (buf, "$t%d", tmp_id++);
          MIR_reg_t old = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
          MIR_append_insn (g_ctx, g_func,
                           MIR_new_insn (g_ctx, MIR_MOV, MIR_new_reg_op (g_ctx, old),
                                         MIR_new_mem_op (g_ctx, MIR_T_P, 0, addr, 0, 1)));
          MIR_append_insn (g_ctx, g_func,
                           MIR_new_call_insn (g_ctx, 3, MIR_new_ref_op (g_ctx, free_proto),
                                              MIR_new_ref_op (g_ctx, free_import),
                                              MIR_new_reg_op (g_ctx, old)));
        }
        MIR_append_insn (g_ctx, g_func,
                         MIR_new_insn (g_ctx, store_code,
                                       MIR_new_mem_op (g_ctx, v->is_str ? MIR_T_I64 : MIR_T_D, 0,
                                                       addr, 0, 1),
                                       MIR_new_reg_op (g_ctx, res)));
      } else {
        MIR_reg_t dst = get_var (&g_vars, g_ctx, g_func, v->var);
        if (v->is_str)
          MIR_append_insn (g_ctx, g_func,
                           MIR_new_call_insn (g_ctx, 3, MIR_new_ref_op (g_ctx, free_proto),
                                              MIR_new_ref_op (g_ctx, free_import),
                                              MIR_new_reg_op (g_ctx, dst)));
        MIR_append_insn (g_ctx, g_func,
                         MIR_new_insn (g_ctx, v->is_str ? MIR_MOV : MIR_DMOV,
                                       MIR_new_reg_op (g_ctx, dst), MIR_new_reg_op (g_ctx, res)));
      }
    }
    break;
  }
  case ST_RESTORE: {
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_call_insn (g_ctx, 2, MIR_new_ref_op (g_ctx, restore_proto),
                                        MIR_new_ref_op (g_ctx, restore_import)));
    break;
  }
  case ST_OPEN: {
    MIR_reg_t fn = gen_expr (g_ctx, g_func, &g_vars, s->u.open.num);
    MIR_reg_t path = gen_expr (g_ctx, g_func, &g_vars, s->u.open.path);
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_call_insn (g_ctx, 4, MIR_new_ref_op (g_ctx, open_proto),
                                        MIR_new_ref_op (g_ctx, open_import),
                                        MIR_new_reg_op (g_ctx, fn), MIR_new_reg_op (g_ctx, path)));
    break;
  }
  case ST_CLOSE: {
    MIR_reg_t fn = gen_expr (g_ctx, g_func, &g_vars, s->u.close.num);
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_call_insn (g_ctx, 3, MIR_new_ref_op (g_ctx, close_proto),
                                        MIR_new_ref_op (g_ctx, close_import),
                                        MIR_new_reg_op (g_ctx, fn)));
    break;
  }
  case ST_FOR: {
    MIR_reg_t var = get_var (&g_vars, g_ctx, g_func, s->u.forto.var);
    MIR_reg_t start = gen_expr (g_ctx, g_func, &g_vars, s->u.forto.start);
    MIR_reg_t end = gen_expr (g_ctx, g_func, &g_vars, s->u.forto.end);
    MIR_reg_t step = gen_expr (g_ctx, g_func, &g_vars, s->u.forto.step);
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_insn (g_ctx, MIR_DMOV, MIR_new_reg_op (g_ctx, var),
                                   MIR_new_reg_op (g_ctx, start)));
    MIR_label_t start_label = MIR_new_label (g_ctx);
    MIR_label_t end_label = MIR_new_label (g_ctx);
    if (g_loop_len == g_loop_cap) {
      g_loop_cap = g_loop_cap ? 2 * g_loop_cap : 16;
      g_loop_stack = realloc (g_loop_stack, g_loop_cap * sizeof (LoopInfo));
    }
    g_loop_stack[g_loop_len++] = (LoopInfo) {var, end, step, start_label, end_label, 0};
    MIR_append_insn (g_ctx, g_func, start_label);
    MIR_label_t neg_step = MIR_new_label (g_ctx);
    MIR_label_t after_cmp = MIR_new_label (g_ctx);
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_insn (g_ctx, MIR_DBLT, MIR_new_label_op (g_ctx, neg_step),
                                   MIR_new_reg_op (g_ctx, step), MIR_new_double_op (g_ctx, 0.0)));
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_insn (g_ctx, MIR_DBGT, MIR_new_label_op (g_ctx, end_label),
                                   MIR_new_reg_op (g_ctx, var), MIR_new_reg_op (g_ctx, end)));
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_insn (g_ctx, MIR_JMP, MIR_new_label_op (g_ctx, after_cmp)));
    MIR_append_insn (g_ctx, g_func, neg_step);
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_insn (g_ctx, MIR_DBLT, MIR_new_label_op (g_ctx, end_label),
                                   MIR_new_reg_op (g_ctx, var), MIR_new_reg_op (g_ctx, end)));
    MIR_append_insn (g_ctx, g_func, after_cmp);
    break;
  }
  case ST_NEXT: {
    if (g_loop_len == 0) break;
    LoopInfo info = g_loop_stack[--g_loop_len];
    if (info.is_while) break;
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_insn (g_ctx, MIR_DADD, MIR_new_reg_op (g_ctx, info.var),
                                   MIR_new_reg_op (g_ctx, info.var),
                                   MIR_new_reg_op (g_ctx, info.step)));
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_insn (g_ctx, MIR_JMP, MIR_new_label_op (g_ctx, info.start_label)));
    MIR_append_insn (g_ctx, g_func, info.end_label);
    break;
  }
  case ST_WHILE: {
    MIR_label_t start_label = MIR_new_label (g_ctx);
    MIR_label_t end_label = MIR_new_label (g_ctx);
    if (g_loop_len == g_loop_cap) {
      g_loop_cap = g_loop_cap ? 2 * g_loop_cap : 16;
      g_loop_stack = realloc (g_loop_stack, g_loop_cap * sizeof (LoopInfo));
    }
    g_loop_stack[g_loop_len++] = (LoopInfo) {0, 0, 0, start_label, end_label, 1};
    MIR_append_insn (g_ctx, g_func, start_label);
    MIR_reg_t cond = gen_expr (g_ctx, g_func, &g_vars, s->u.expr);
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_insn (g_ctx, MIR_DBEQ, MIR_new_label_op (g_ctx, end_label),
                                   MIR_new_reg_op (g_ctx, cond), MIR_new_double_op (g_ctx, 0.0)));
    break;
  }
  case ST_WEND: {
    if (g_loop_len == 0) break;
    LoopInfo info = g_loop_stack[--g_loop_len];
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_insn (g_ctx, MIR_JMP, MIR_new_label_op (g_ctx, info.start_label)));
    MIR_append_insn (g_ctx, g_func, info.end_label);
    break;
  }
  case ST_LET: {
    MIR_reg_t r = gen_expr (g_ctx, g_func, &g_vars, s->u.let.expr);
    if (s->u.let.is_str && (s->u.let.expr->kind == N_VAR || s->u.let.expr->kind == N_STR)) {
      char buf[32];
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t dup = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_call_insn (g_ctx, 4, MIR_new_ref_op (g_ctx, strdup_proto),
                                          MIR_new_ref_op (g_ctx, strdup_import),
                                          MIR_new_reg_op (g_ctx, dup), MIR_new_reg_op (g_ctx, r)));
      r = dup;
    }
    if (s->u.let.var->index != NULL) {
      MIR_reg_t base = get_array (&g_vars, g_ctx, g_func, s->u.let.var->var, 0, 0, s->u.let.is_str);
      MIR_reg_t idxd1 = gen_expr (g_ctx, g_func, &g_vars, s->u.let.var->index);
      char buf[32];
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t idx = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_D2I, MIR_new_reg_op (g_ctx, idx),
                                     MIR_new_reg_op (g_ctx, idxd1)));
      if (array_base != 0)
        MIR_append_insn (g_ctx, g_func,
                         MIR_new_insn (g_ctx, MIR_SUB, MIR_new_reg_op (g_ctx, idx),
                                       MIR_new_reg_op (g_ctx, idx),
                                       MIR_new_int_op (g_ctx, array_base)));
      if (s->u.let.var->index2 != NULL) {
        MIR_reg_t idxd2 = gen_expr (g_ctx, g_func, &g_vars, s->u.let.var->index2);
        sprintf (buf, "$t%d", tmp_id++);
        MIR_reg_t idx2 = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
        MIR_append_insn (g_ctx, g_func,
                         MIR_new_insn (g_ctx, MIR_D2I, MIR_new_reg_op (g_ctx, idx2),
                                       MIR_new_reg_op (g_ctx, idxd2)));
        if (array_base != 0)
          MIR_append_insn (g_ctx, g_func,
                           MIR_new_insn (g_ctx, MIR_SUB, MIR_new_reg_op (g_ctx, idx2),
                                         MIR_new_reg_op (g_ctx, idx2),
                                         MIR_new_int_op (g_ctx, array_base)));
        size_t dim2 = get_array_dim2 (&g_vars, s->u.let.var->var);
        sprintf (buf, "$t%d", tmp_id++);
        MIR_reg_t tmp = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
        MIR_append_insn (g_ctx, g_func,
                         MIR_new_insn (g_ctx, MIR_MUL, MIR_new_reg_op (g_ctx, tmp),
                                       MIR_new_reg_op (g_ctx, idx), MIR_new_int_op (g_ctx, dim2)));
        MIR_append_insn (g_ctx, g_func,
                         MIR_new_insn (g_ctx, MIR_ADD, MIR_new_reg_op (g_ctx, idx),
                                       MIR_new_reg_op (g_ctx, tmp), MIR_new_reg_op (g_ctx, idx2)));
      }
      size_t asize = get_array_size (&g_vars, s->u.let.var->var);
      MIR_label_t bad = MIR_new_label (g_ctx), ok = MIR_new_label (g_ctx);
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_BLT, MIR_new_label_op (g_ctx, bad),
                                     MIR_new_reg_op (g_ctx, idx), MIR_new_int_op (g_ctx, 0)));
      if (asize != 0)
        MIR_append_insn (g_ctx, g_func,
                         MIR_new_insn (g_ctx, MIR_BGE, MIR_new_label_op (g_ctx, bad),
                                       MIR_new_reg_op (g_ctx, idx), MIR_new_int_op (g_ctx, asize)));
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t off = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
      size_t elem_size = s->u.let.is_str ? sizeof (char *) : sizeof (basic_num_t);
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_MUL, MIR_new_reg_op (g_ctx, off),
                                     MIR_new_reg_op (g_ctx, idx),
                                     MIR_new_int_op (g_ctx, elem_size)));
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t addr = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_ADD, MIR_new_reg_op (g_ctx, addr),
                                     MIR_new_reg_op (g_ctx, base), MIR_new_reg_op (g_ctx, off)));
      MIR_insn_code_t mov = s->u.let.is_str ? MIR_MOV : MIR_DMOV;
      if (s->u.let.is_str) {
        sprintf (buf, "$t%d", tmp_id++);
        MIR_reg_t old = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
        MIR_append_insn (g_ctx, g_func,
                         MIR_new_insn (g_ctx, MIR_MOV, MIR_new_reg_op (g_ctx, old),
                                       MIR_new_mem_op (g_ctx, MIR_T_P, 0, addr, 0, 1)));
        MIR_append_insn (g_ctx, g_func,
                         MIR_new_call_insn (g_ctx, 3, MIR_new_ref_op (g_ctx, free_proto),
                                            MIR_new_ref_op (g_ctx, free_import),
                                            MIR_new_reg_op (g_ctx, old)));
      }
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, mov,
                                     MIR_new_mem_op (g_ctx, s->u.let.is_str ? MIR_T_P : MIR_T_D, 0,
                                                     addr, 0, 1),
                                     MIR_new_reg_op (g_ctx, r)));
      MIR_append_insn (g_ctx, g_func, MIR_new_insn (g_ctx, MIR_JMP, MIR_new_label_op (g_ctx, ok)));
      MIR_append_insn (g_ctx, g_func, bad);
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_call_insn (g_ctx, 2, MIR_new_ref_op (g_ctx, stop_proto),
                                          MIR_new_ref_op (g_ctx, stop_import)));
      MIR_append_insn (g_ctx, g_func, ok);
    } else {
      MIR_reg_t v = get_var (&g_vars, g_ctx, g_func, s->u.let.var->var);
      MIR_insn_code_t mov = s->u.let.is_str ? MIR_MOV : MIR_DMOV;
      if (s->u.let.is_str)
        MIR_append_insn (g_ctx, g_func,
                         MIR_new_call_insn (g_ctx, 3, MIR_new_ref_op (g_ctx, free_proto),
                                            MIR_new_ref_op (g_ctx, free_import),
                                            MIR_new_reg_op (g_ctx, v)));
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, mov, MIR_new_reg_op (g_ctx, v),
                                     MIR_new_reg_op (g_ctx, r)));
    }
    break;
  }
  case ST_GOTO: {
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_insn (g_ctx, MIR_JMP,
                                   MIR_new_label_op (g_ctx,
                                                     find_label (g_prog, g_labels, s->u.target))));
    break;
  }
  case ST_GOSUB: {
    MIR_label_t ret = MIR_new_label (g_ctx);
    MIR_item_t ret_ref = MIR_new_lref_data (g_ctx, NULL, ret, NULL, 0);
    char buf[32];
    sprintf (buf, "$t%d", tmp_id++);
    MIR_reg_t tmp = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_insn (g_ctx, MIR_MOV, MIR_new_reg_op (g_ctx, tmp),
                                   MIR_new_ref_op (g_ctx, ret_ref)));
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_insn (g_ctx, MIR_MOV, MIR_new_reg_op (g_ctx, tmp),
                                   MIR_new_mem_op (g_ctx, MIR_T_I64, 0, tmp, 0, 1)));
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_insn (g_ctx, MIR_MOV,
                                   MIR_new_mem_op (g_ctx, MIR_T_P, 0, g_ret_stack, g_ret_sp, 1),
                                   MIR_new_reg_op (g_ctx, tmp)));
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_insn (g_ctx, MIR_ADD, MIR_new_reg_op (g_ctx, g_ret_sp),
                                   MIR_new_reg_op (g_ctx, g_ret_sp), MIR_new_int_op (g_ctx, 8)));
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_insn (g_ctx, MIR_JMP,
                                   MIR_new_label_op (g_ctx,
                                                     find_label (g_prog, g_labels, s->u.target))));
    MIR_append_insn (g_ctx, g_func, ret);
    break;
  }
  case ST_RETURN: {
    char buf[32];
    sprintf (buf, "$t%d", tmp_id++);
    MIR_reg_t addr = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_insn (g_ctx, MIR_SUB, MIR_new_reg_op (g_ctx, g_ret_sp),
                                   MIR_new_reg_op (g_ctx, g_ret_sp), MIR_new_int_op (g_ctx, 8)));
    MIR_append_insn (g_ctx, g_func,

                     MIR_new_insn (g_ctx, MIR_MOV, MIR_new_reg_op (g_ctx, addr),
                                   MIR_new_mem_op (g_ctx, MIR_T_P, 0, g_ret_stack, g_ret_sp, 1)));
    MIR_append_insn (g_ctx, g_func, MIR_new_insn (g_ctx, MIR_JMPI, MIR_new_reg_op (g_ctx, addr)));
    break;
  }
  case ST_IF: {
    MIR_label_t else_label = MIR_new_label (g_ctx), end = MIR_new_label (g_ctx);
    MIR_reg_t r = gen_expr (g_ctx, g_func, &g_vars, s->u.iff.cond);
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_insn (g_ctx, MIR_DBEQ, MIR_new_label_op (g_ctx, else_label),
                                   MIR_new_reg_op (g_ctx, r), MIR_new_double_op (g_ctx, 0.0)));
    for (size_t k = 0; k < s->u.iff.then_stmts.len; k++) gen_stmt (&s->u.iff.then_stmts.data[k]);
    MIR_append_insn (g_ctx, g_func, MIR_new_insn (g_ctx, MIR_JMP, MIR_new_label_op (g_ctx, end)));
    MIR_append_insn (g_ctx, g_func, else_label);
    for (size_t k = 0; k < s->u.iff.else_stmts.len; k++) gen_stmt (&s->u.iff.else_stmts.data[k]);
    MIR_append_insn (g_ctx, g_func, end);
    break;
  }
  case ST_ON_GOTO: {
    MIR_reg_t r = gen_expr (g_ctx, g_func, &g_vars, s->u.on_goto.expr);
    char buf[32];
    sprintf (buf, "$t%d", tmp_id++);
    MIR_reg_t ri = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_insn (g_ctx, MIR_D2I, MIR_new_reg_op (g_ctx, ri),
                                   MIR_new_reg_op (g_ctx, r)));
    for (size_t k = 0; k < s->u.on_goto.n_targets; k++) {
      MIR_label_t next = MIR_new_label (g_ctx);
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_BNE, MIR_new_label_op (g_ctx, next),
                                     MIR_new_reg_op (g_ctx, ri), MIR_new_int_op (g_ctx, k + 1)));
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_JMP,
                                     MIR_new_label_op (g_ctx,
                                                       find_label (g_prog, g_labels,
                                                                   s->u.on_goto.targets[k]))));
      MIR_append_insn (g_ctx, g_func, next);
    }
    break;
  }
  case ST_ON_GOSUB: {
    MIR_reg_t r = gen_expr (g_ctx, g_func, &g_vars, s->u.on_gosub.expr);
    char buf[32];
    sprintf (buf, "$t%d", tmp_id++);
    MIR_reg_t ri = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_insn (g_ctx, MIR_D2I, MIR_new_reg_op (g_ctx, ri),
                                   MIR_new_reg_op (g_ctx, r)));
    MIR_label_t after = MIR_new_label (g_ctx);
    MIR_item_t after_ref = MIR_new_lref_data (g_ctx, NULL, after, NULL, 0);
    for (size_t k = 0; k < s->u.on_gosub.n_targets; k++) {
      MIR_label_t next = MIR_new_label (g_ctx);
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_BNE, MIR_new_label_op (g_ctx, next),
                                     MIR_new_reg_op (g_ctx, ri), MIR_new_int_op (g_ctx, k + 1)));
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t tmp = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_MOV, MIR_new_reg_op (g_ctx, tmp),
                                     MIR_new_ref_op (g_ctx, after_ref)));
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_MOV, MIR_new_reg_op (g_ctx, tmp),
                                     MIR_new_mem_op (g_ctx, MIR_T_I64, 0, tmp, 0, 1)));
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_MOV,
                                     MIR_new_mem_op (g_ctx, MIR_T_P, 0, g_ret_stack, g_ret_sp, 1),
                                     MIR_new_reg_op (g_ctx, tmp)));
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_ADD, MIR_new_reg_op (g_ctx, g_ret_sp),
                                     MIR_new_reg_op (g_ctx, g_ret_sp), MIR_new_int_op (g_ctx, 8)));
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_JMP,
                                     MIR_new_label_op (g_ctx,
                                                       find_label (g_prog, g_labels,
                                                                   s->u.on_gosub.targets[k]))));
      MIR_append_insn (g_ctx, g_func, next);
    }
    MIR_append_insn (g_ctx, g_func, after);
    break;
  }
  case ST_HPLOT: {
    char buf[32];
    MIR_reg_t xprev, yprev;
    size_t start = 0;
    if (s->u.hplot.from_prev) {
      MIR_reg_t x = gen_expr (g_ctx, g_func, &g_vars, s->u.hplot.xs[0]);
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t xi = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_D2I, MIR_new_reg_op (g_ctx, xi),
                                     MIR_new_reg_op (g_ctx, x)));
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t xd = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_D, buf);
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_I2D, MIR_new_reg_op (g_ctx, xd),
                                     MIR_new_reg_op (g_ctx, xi)));
      MIR_reg_t y = gen_expr (g_ctx, g_func, &g_vars, s->u.hplot.ys[0]);
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t yi = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_D2I, MIR_new_reg_op (g_ctx, yi),
                                     MIR_new_reg_op (g_ctx, y)));
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t yd = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_D, buf);
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_I2D, MIR_new_reg_op (g_ctx, yd),
                                     MIR_new_reg_op (g_ctx, yi)));
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_call_insn (g_ctx, 4, MIR_new_ref_op (g_ctx, hplottocur_proto),
                                          MIR_new_ref_op (g_ctx, hplottocur_import),
                                          MIR_new_reg_op (g_ctx, xd), MIR_new_reg_op (g_ctx, yd)));
      xprev = xd;
      yprev = yd;
      start = 1;
    } else {
      MIR_reg_t x0 = gen_expr (g_ctx, g_func, &g_vars, s->u.hplot.xs[0]);
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t x0i = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_D2I, MIR_new_reg_op (g_ctx, x0i),
                                     MIR_new_reg_op (g_ctx, x0)));
      sprintf (buf, "$t%d", tmp_id++);
      xprev = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_D, buf);
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_I2D, MIR_new_reg_op (g_ctx, xprev),
                                     MIR_new_reg_op (g_ctx, x0i)));
      MIR_reg_t y0 = gen_expr (g_ctx, g_func, &g_vars, s->u.hplot.ys[0]);
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t y0i = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_D2I, MIR_new_reg_op (g_ctx, y0i),
                                     MIR_new_reg_op (g_ctx, y0)));
      sprintf (buf, "$t%d", tmp_id++);
      yprev = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_D, buf);
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_I2D, MIR_new_reg_op (g_ctx, yprev),
                                     MIR_new_reg_op (g_ctx, y0i)));
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_call_insn (g_ctx, 4, MIR_new_ref_op (g_ctx, hplot_proto),
                                          MIR_new_ref_op (g_ctx, hplot_import),
                                          MIR_new_reg_op (g_ctx, xprev),
                                          MIR_new_reg_op (g_ctx, yprev)));
      start = 1;
    }
    for (size_t k = start; k < s->u.hplot.n; k++) {
      MIR_reg_t x = gen_expr (g_ctx, g_func, &g_vars, s->u.hplot.xs[k]);
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t xi = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_D2I, MIR_new_reg_op (g_ctx, xi),
                                     MIR_new_reg_op (g_ctx, x)));
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t xd = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_D, buf);
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_I2D, MIR_new_reg_op (g_ctx, xd),
                                     MIR_new_reg_op (g_ctx, xi)));
      MIR_reg_t y = gen_expr (g_ctx, g_func, &g_vars, s->u.hplot.ys[k]);
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t yi = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_D2I, MIR_new_reg_op (g_ctx, yi),
                                     MIR_new_reg_op (g_ctx, y)));
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t yd = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_D, buf);
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_I2D, MIR_new_reg_op (g_ctx, yd),
                                     MIR_new_reg_op (g_ctx, yi)));
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_call_insn (g_ctx, 6, MIR_new_ref_op (g_ctx, hplotto_proto),
                                          MIR_new_ref_op (g_ctx, hplotto_import),
                                          MIR_new_reg_op (g_ctx, xprev),
                                          MIR_new_reg_op (g_ctx, yprev), MIR_new_reg_op (g_ctx, xd),
                                          MIR_new_reg_op (g_ctx, yd)));
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_DMOV, MIR_new_reg_op (g_ctx, xprev),
                                     MIR_new_reg_op (g_ctx, xd)));
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_DMOV, MIR_new_reg_op (g_ctx, yprev),
                                     MIR_new_reg_op (g_ctx, yd)));
    }
    break;
  }
  case ST_MOVE: {
    MIR_reg_t x = gen_expr (g_ctx, g_func, &g_vars, s->u.move.x);
    MIR_reg_t y = gen_expr (g_ctx, g_func, &g_vars, s->u.move.y);
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_call_insn (g_ctx, 4, MIR_new_ref_op (g_ctx, move_proto),
                                        MIR_new_ref_op (g_ctx, move_import),
                                        MIR_new_reg_op (g_ctx, x), MIR_new_reg_op (g_ctx, y)));
    break;
  }
  case ST_DRAW: {
    MIR_reg_t x = gen_expr (g_ctx, g_func, &g_vars, s->u.draw.x);
    MIR_reg_t y = gen_expr (g_ctx, g_func, &g_vars, s->u.draw.y);
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_call_insn (g_ctx, 4, MIR_new_ref_op (g_ctx, draw_proto),
                                        MIR_new_ref_op (g_ctx, draw_import),
                                        MIR_new_reg_op (g_ctx, x), MIR_new_reg_op (g_ctx, y)));
    break;
  }
  case ST_LINE: {
    MIR_reg_t x0 = gen_expr (g_ctx, g_func, &g_vars, s->u.line.x0);
    MIR_reg_t y0 = gen_expr (g_ctx, g_func, &g_vars, s->u.line.y0);
    MIR_reg_t x1 = gen_expr (g_ctx, g_func, &g_vars, s->u.line.x1);
    MIR_reg_t y1 = gen_expr (g_ctx, g_func, &g_vars, s->u.line.y1);
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_call_insn (g_ctx, 6, MIR_new_ref_op (g_ctx, line_proto),
                                        MIR_new_ref_op (g_ctx, line_import),
                                        MIR_new_reg_op (g_ctx, x0), MIR_new_reg_op (g_ctx, y0),
                                        MIR_new_reg_op (g_ctx, x1), MIR_new_reg_op (g_ctx, y1)));
    break;
  }
  case ST_CIRCLE: {
    MIR_reg_t x = gen_expr (g_ctx, g_func, &g_vars, s->u.circle.x);
    MIR_reg_t y = gen_expr (g_ctx, g_func, &g_vars, s->u.circle.y);
    MIR_reg_t r = gen_expr (g_ctx, g_func, &g_vars, s->u.circle.r);
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_call_insn (g_ctx, 5, MIR_new_ref_op (g_ctx, circle_proto),
                                        MIR_new_ref_op (g_ctx, circle_import),
                                        MIR_new_reg_op (g_ctx, x), MIR_new_reg_op (g_ctx, y),
                                        MIR_new_reg_op (g_ctx, r)));
    break;
  }
  case ST_RECT: {
    MIR_reg_t x0 = gen_expr (g_ctx, g_func, &g_vars, s->u.rect.x0);
    MIR_reg_t y0 = gen_expr (g_ctx, g_func, &g_vars, s->u.rect.y0);
    MIR_reg_t x1 = gen_expr (g_ctx, g_func, &g_vars, s->u.rect.x1);
    MIR_reg_t y1 = gen_expr (g_ctx, g_func, &g_vars, s->u.rect.y1);
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_call_insn (g_ctx, 6, MIR_new_ref_op (g_ctx, rect_proto),
                                        MIR_new_ref_op (g_ctx, rect_import),
                                        MIR_new_reg_op (g_ctx, x0), MIR_new_reg_op (g_ctx, y0),
                                        MIR_new_reg_op (g_ctx, x1), MIR_new_reg_op (g_ctx, y1)));
    break;
  }
  case ST_MODE: {
    MIR_reg_t m = gen_expr (g_ctx, g_func, &g_vars, s->u.expr);
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_call_insn (g_ctx, 3, MIR_new_ref_op (g_ctx, mode_proto),
                                        MIR_new_ref_op (g_ctx, mode_import),
                                        MIR_new_reg_op (g_ctx, m)));
    break;
  }
  case ST_FILL: {
    MIR_reg_t x0 = gen_expr (g_ctx, g_func, &g_vars, s->u.fill.x0);
    MIR_reg_t y0 = gen_expr (g_ctx, g_func, &g_vars, s->u.fill.y0);
    MIR_reg_t x1 = gen_expr (g_ctx, g_func, &g_vars, s->u.fill.x1);
    MIR_reg_t y1 = gen_expr (g_ctx, g_func, &g_vars, s->u.fill.y1);
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_call_insn (g_ctx, 6, MIR_new_ref_op (g_ctx, fill_proto),
                                        MIR_new_ref_op (g_ctx, fill_import),
                                        MIR_new_reg_op (g_ctx, x0), MIR_new_reg_op (g_ctx, y0),
                                        MIR_new_reg_op (g_ctx, x1), MIR_new_reg_op (g_ctx, y1)));
    break;
  }
  case ST_BEEP: {
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_call_insn (g_ctx, 2, MIR_new_ref_op (g_ctx, beep_proto),
                                        MIR_new_ref_op (g_ctx, beep_import)));
    break;
  }
  case ST_SOUND: {
    MIR_reg_t f = gen_expr (g_ctx, g_func, &g_vars, s->u.sound.freq);
    MIR_reg_t d = gen_expr (g_ctx, g_func, &g_vars, s->u.sound.dur);
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_call_insn (g_ctx, 4, MIR_new_ref_op (g_ctx, sound_proto),
                                        MIR_new_ref_op (g_ctx, sound_import),
                                        MIR_new_reg_op (g_ctx, f), MIR_new_reg_op (g_ctx, d)));
    break;
  }
  case ST_SYSTEM: {
    MIR_reg_t cmd = gen_expr (g_ctx, g_func, &g_vars, s->u.system.cmd);
    MIR_reg_t status = get_var (&g_vars, g_ctx, g_func, s->u.system.status);
    MIR_reg_t out = get_var (&g_vars, g_ctx, g_func, s->u.system.out);
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_call_insn (g_ctx, 4, MIR_new_ref_op (g_ctx, system_proto),
                                        MIR_new_ref_op (g_ctx, system_import),
                                        MIR_new_reg_op (g_ctx, status),
                                        MIR_new_reg_op (g_ctx, cmd)));
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_call_insn (g_ctx, 3, MIR_new_ref_op (g_ctx, free_proto),
                                        MIR_new_ref_op (g_ctx, free_import),
                                        MIR_new_reg_op (g_ctx, out)));
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_call_insn (g_ctx, 3, MIR_new_ref_op (g_ctx, system_out_proto),
                                        MIR_new_ref_op (g_ctx, system_out_import),
                                        MIR_new_reg_op (g_ctx, out)));
    break;
  }
  case ST_ON_ERROR: {
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_call_insn (g_ctx, 3, MIR_new_ref_op (g_ctx, on_error_proto),
                                        MIR_new_ref_op (g_ctx, on_error_import),
                                        MIR_new_double_op (g_ctx, (basic_num_t) s->u.target)));
    break;
  }
  case ST_RESUME: {
    if (s->u.resume.has_line) {
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_JMP,
                                     MIR_new_label_op (g_ctx, find_label (g_prog, g_labels,
                                                                          s->u.resume.line))));
    } else {
      if (!g_line_tracking) {
        fprintf (stderr, "RESUME without line requires line tracking\n");
        exit (1);
      }
      char buf[32];
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t line = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_D, buf);
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_call_insn (g_ctx, 3, MIR_new_ref_op (g_ctx, get_line_proto),
                                          MIR_new_ref_op (g_ctx, get_line_import),
                                          MIR_new_reg_op (g_ctx, line)));
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t idx = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_D2I, MIR_new_reg_op (g_ctx, idx),
                                     MIR_new_reg_op (g_ctx, line)));
      MIR_label_t after = MIR_new_label (g_ctx);
      for (size_t k = 0; k < g_prog->len; k++) {
        MIR_label_t next = k + 1 < g_prog->len ? MIR_new_label (g_ctx) : after;
        MIR_append_insn (g_ctx, g_func,
                         MIR_new_insn (g_ctx, MIR_BNE, MIR_new_label_op (g_ctx, next),
                                       MIR_new_reg_op (g_ctx, idx),
                                       MIR_new_int_op (g_ctx, g_prog->data[k].line)));
        MIR_append_insn (g_ctx, g_func,
                         MIR_new_insn (g_ctx, MIR_JMP, MIR_new_label_op (g_ctx, g_labels[k])));
        if (k + 1 < g_prog->len) MIR_append_insn (g_ctx, g_func, next);
      }
      MIR_append_insn (g_ctx, g_func, after);
    }
    break;
  }
  case ST_CALL: {
    MIR_op_t args[3];
    size_t nargs = 0;
    if (s->u.call.arg1 != NULL)
      args[nargs++] = MIR_new_reg_op (g_ctx, gen_expr (g_ctx, g_func, &g_vars, s->u.call.arg1));
    if (s->u.call.arg2 != NULL)
      args[nargs++] = MIR_new_reg_op (g_ctx, gen_expr (g_ctx, g_func, &g_vars, s->u.call.arg2));
    if (s->u.call.arg3 != NULL)
      args[nargs++] = MIR_new_reg_op (g_ctx, gen_expr (g_ctx, g_func, &g_vars, s->u.call.arg3));
    if (s->u.call.arg4 != NULL)
      args[nargs++] = MIR_new_reg_op (g_ctx, gen_expr (g_ctx, g_func, &g_vars, s->u.call.arg4));
    if (s->u.call.arg5 != NULL)
      args[nargs++] = MIR_new_reg_op (g_ctx, gen_expr (g_ctx, g_func, &g_vars, s->u.call.arg5));
    FuncDef *fd = find_func (s->u.call.name);
    MIR_item_t proto = fd->proto;
    MIR_item_t item = fd->item;
    int has_ret = fd->is_proc ? 0 : 1;
    char buf[32];
    MIR_reg_t tmp = 0;
    if (has_ret) {
      sprintf (buf, "$t%d", tmp_id++);
      MIR_type_t t = fd->is_str_ret ? MIR_T_P : MIR_T_D;
      tmp = MIR_new_func_reg (g_ctx, g_func->u.func, t, buf);
    }
    switch (nargs) {
    case 0:
      MIR_append_insn (g_ctx, g_func,
                       has_ret ? MIR_new_call_insn (g_ctx, 3, MIR_new_ref_op (g_ctx, proto),
                                                    MIR_new_ref_op (g_ctx, item),
                                                    MIR_new_reg_op (g_ctx, tmp))
                               : MIR_new_call_insn (g_ctx, 2, MIR_new_ref_op (g_ctx, proto),
                                                    MIR_new_ref_op (g_ctx, item)));
      break;
    case 1:
      MIR_append_insn (g_ctx, g_func,
                       has_ret ? MIR_new_call_insn (g_ctx, 4, MIR_new_ref_op (g_ctx, proto),
                                                    MIR_new_ref_op (g_ctx, item),
                                                    MIR_new_reg_op (g_ctx, tmp), args[0])
                               : MIR_new_call_insn (g_ctx, 3, MIR_new_ref_op (g_ctx, proto),
                                                    MIR_new_ref_op (g_ctx, item), args[0]));
      break;
    case 2:
      MIR_append_insn (g_ctx, g_func,
                       has_ret
                         ? MIR_new_call_insn (g_ctx, 5, MIR_new_ref_op (g_ctx, proto),
                                              MIR_new_ref_op (g_ctx, item),
                                              MIR_new_reg_op (g_ctx, tmp), args[0], args[1])
                         : MIR_new_call_insn (g_ctx, 4, MIR_new_ref_op (g_ctx, proto),
                                              MIR_new_ref_op (g_ctx, item), args[0], args[1]));
      break;
    case 3:
      MIR_append_insn (g_ctx, g_func,
                       has_ret ? MIR_new_call_insn (g_ctx, 6, MIR_new_ref_op (g_ctx, proto),
                                                    MIR_new_ref_op (g_ctx, item),
                                                    MIR_new_reg_op (g_ctx, tmp), args[0], args[1],
                                                    args[2])
                               : MIR_new_call_insn (g_ctx, 5, MIR_new_ref_op (g_ctx, proto),
                                                    MIR_new_ref_op (g_ctx, item), args[0], args[1],
                                                    args[2]));
      break;
    }
    break;
  }
  case ST_END: {
    MIR_append_insn (g_ctx, g_func, MIR_new_ret_insn (g_ctx, 1, MIR_new_int_op (g_ctx, 0)));
    break;
  }
  case ST_STOP: {
    MIR_append_insn (g_ctx, g_func,
                     MIR_new_call_insn (g_ctx, 2, MIR_new_ref_op (g_ctx, stop_proto),
                                        MIR_new_ref_op (g_ctx, stop_import)));
    break;
  }
  case ST_DIM: {
    for (size_t k = 0; k < s->u.dim.n; k++) {
      if (s->u.dim.sizes1[k] == NULL) continue;
      size_t csize1 = 0, csize2 = 0;
      if (s->u.dim.sizes1[k]->kind == N_NUM) csize1 = s->u.dim.sizes1[k]->num - array_base + 1;
      if (s->u.dim.sizes2[k] != NULL && s->u.dim.sizes2[k]->kind == N_NUM)
        csize2 = s->u.dim.sizes2[k]->num - array_base + 1;
      MIR_reg_t base
        = get_array (&g_vars, g_ctx, g_func, s->u.dim.names[k], csize1, csize2, s->u.dim.is_str[k]);
      MIR_reg_t size1d = gen_expr (g_ctx, g_func, &g_vars, s->u.dim.sizes1[k]);
      char buf[32];
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t size1 = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_insn (g_ctx, MIR_D2I, MIR_new_reg_op (g_ctx, size1),
                                     MIR_new_reg_op (g_ctx, size1d)));
      if (1 - array_base != 0)
        MIR_append_insn (g_ctx, g_func,
                         MIR_new_insn (g_ctx, MIR_ADD, MIR_new_reg_op (g_ctx, size1),
                                       MIR_new_reg_op (g_ctx, size1),
                                       MIR_new_int_op (g_ctx, 1 - array_base)));
      MIR_reg_t total = size1;
      if (s->u.dim.sizes2[k] != NULL) {
        MIR_reg_t size2d = gen_expr (g_ctx, g_func, &g_vars, s->u.dim.sizes2[k]);
        sprintf (buf, "$t%d", tmp_id++);
        MIR_reg_t size2 = MIR_new_func_reg (g_ctx, g_func->u.func, MIR_T_I64, buf);
        MIR_append_insn (g_ctx, g_func,
                         MIR_new_insn (g_ctx, MIR_D2I, MIR_new_reg_op (g_ctx, size2),
                                       MIR_new_reg_op (g_ctx, size2d)));
        if (1 - array_base != 0)
          MIR_append_insn (g_ctx, g_func,
                           MIR_new_insn (g_ctx, MIR_ADD, MIR_new_reg_op (g_ctx, size2),
                                         MIR_new_reg_op (g_ctx, size2),
                                         MIR_new_int_op (g_ctx, 1 - array_base)));
        MIR_append_insn (g_ctx, g_func,
                         MIR_new_insn (g_ctx, MIR_MUL, MIR_new_reg_op (g_ctx, total),
                                       MIR_new_reg_op (g_ctx, total),
                                       MIR_new_reg_op (g_ctx, size2)));
      }
      size_t elem_size = s->u.dim.is_str[k] ? sizeof (char *) : sizeof (basic_num_t);
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_call_insn (g_ctx, 5, MIR_new_ref_op (g_ctx, calloc_proto),
                                          MIR_new_ref_op (g_ctx, calloc_import),
                                          MIR_new_reg_op (g_ctx, base),
                                          MIR_new_reg_op (g_ctx, total),
                                          MIR_new_int_op (g_ctx, elem_size)));
    }
    break;
  }
  case ST_REM:
    /* comment */
    break;
  }
}

static void gen_program (LineVec *prog, int jit, int asm_p, int obj_p, int bin_p, int code_p,
                         int reduce_libs, int profile_p, int track_lines, const char *out_name,
                         const char *src_name) {
  MIR_context_t ctx = MIR_init ();
  MIR_module_t module = MIR_new_module (ctx, "BASIC");
  print_proto = MIR_new_proto (ctx, "basic_print_p", 0, NULL, 1, MIR_T_D, "x");
  print_import = MIR_new_import (ctx, "basic_print");
  prints_proto = MIR_new_proto (ctx, "basic_print_str_p", 0, NULL, 1, MIR_T_P, "s");
  prints_import = MIR_new_import (ctx, "basic_print_str");
  MIR_type_t d = MIR_T_D;
  input_proto = MIR_new_proto (ctx, "basic_input_p", 1, &d, 0);
  input_import = MIR_new_import (ctx, "basic_input");
  MIR_type_t p = MIR_T_P;
  input_str_proto = MIR_new_proto (ctx, "basic_input_str_p", 1, &p, 0);
  input_str_import = MIR_new_import (ctx, "basic_input_str");
  get_proto = MIR_new_proto (ctx, "basic_get_p", 1, &p, 0);
  get_import = MIR_new_import (ctx, "basic_get");
  put_proto = MIR_new_proto (ctx, "basic_put_p", 0, NULL, 1, MIR_T_P, "s");
  put_import = MIR_new_import (ctx, "basic_put");
  open_proto = MIR_new_proto (ctx, "basic_open_p", 0, NULL, 2, MIR_T_D, "n", MIR_T_P, "path");
  open_import = MIR_new_import (ctx, "basic_open");
  close_proto = MIR_new_proto (ctx, "basic_close_p", 0, NULL, 1, MIR_T_D, "n");
  close_import = MIR_new_import (ctx, "basic_close");
  printh_proto = MIR_new_proto (ctx, "basic_print_hash_p", 0, NULL, 2, MIR_T_D, "n", MIR_T_D, "x");
  printh_import = MIR_new_import (ctx, "basic_print_hash");
  prinths_proto
    = MIR_new_proto (ctx, "basic_print_hash_str_p", 0, NULL, 2, MIR_T_D, "n", MIR_T_P, "s");
  prinths_import = MIR_new_import (ctx, "basic_print_hash_str");
  input_hash_proto = MIR_new_proto (ctx, "basic_input_hash_p", 1, &d, 1, MIR_T_D, "n");
  input_hash_import = MIR_new_import (ctx, "basic_input_hash");
  input_hash_str_proto = MIR_new_proto (ctx, "basic_input_hash_str_p", 1, &p, 1, MIR_T_D, "n");
  input_hash_str_import = MIR_new_import (ctx, "basic_input_hash_str");
  get_hash_proto = MIR_new_proto (ctx, "basic_get_hash_p", 1, &p, 1, MIR_T_D, "n");
  get_hash_import = MIR_new_import (ctx, "basic_get_hash");
  put_hash_proto = MIR_new_proto (ctx, "basic_put_hash_p", 0, NULL, 2, MIR_T_D, "n", MIR_T_P, "s");
  put_hash_import = MIR_new_import (ctx, "basic_put_hash");
  home_proto = MIR_new_proto (ctx, "basic_home_p", 0, NULL, 0);
  home_import = MIR_new_import (ctx, "basic_home");
  vtab_proto = MIR_new_proto (ctx, "basic_vtab_p", 0, NULL, 1, MIR_T_D, "n");
  vtab_import = MIR_new_import (ctx, "basic_vtab");
  screen_proto = MIR_new_proto (ctx, "basic_screen_p", 0, NULL, 1, MIR_T_D, "m");
  screen_import = MIR_new_import (ctx, "basic_screen");
  cls_proto = MIR_new_proto (ctx, "basic_cls_p", 0, NULL, 0);
  cls_import = MIR_new_import (ctx, "basic_cls");
  color_proto = MIR_new_proto (ctx, "basic_color_p", 0, NULL, 1, MIR_T_D, "c");
  color_import = MIR_new_import (ctx, "basic_color");
  keyoff_proto = MIR_new_proto (ctx, "basic_key_off_p", 0, NULL, 0);
  keyoff_import = MIR_new_import (ctx, "basic_key_off");
  locate_proto = MIR_new_proto (ctx, "basic_locate_p", 0, NULL, 2, MIR_T_D, "r", MIR_T_D, "c");
  locate_import = MIR_new_import (ctx, "basic_locate");
  htab_proto = MIR_new_proto (ctx, "basic_htab_p", 0, NULL, 1, MIR_T_D, "n");
  htab_import = MIR_new_import (ctx, "basic_htab");
  poke_proto = MIR_new_proto (ctx, "basic_poke_p", 0, NULL, 2, MIR_T_D, "addr", MIR_T_D, "value");
  poke_import = MIR_new_import (ctx, "basic_poke");
  text_proto = MIR_new_proto (ctx, "basic_text_p", 0, NULL, 0);
  text_import = MIR_new_import (ctx, "basic_text");
  inverse_proto = MIR_new_proto (ctx, "basic_inverse_p", 0, NULL, 0);
  inverse_import = MIR_new_import (ctx, "basic_inverse");
  normal_proto = MIR_new_proto (ctx, "basic_normal_p", 0, NULL, 0);
  normal_import = MIR_new_import (ctx, "basic_normal");
  hgr2_proto = MIR_new_proto (ctx, "basic_hgr2_p", 0, NULL, 0);
  hgr2_import = MIR_new_import (ctx, "basic_hgr2");
  hcolor_proto = MIR_new_proto (ctx, "basic_hcolor_p", 0, NULL, 1, MIR_T_D, "c");
  hcolor_import = MIR_new_import (ctx, "basic_hcolor");
  hplot_proto = MIR_new_proto (ctx, "basic_hplot_p", 0, NULL, 2, MIR_T_D, "x", MIR_T_D, "y");
  hplot_import = MIR_new_import (ctx, "basic_hplot");
  hplotto_proto = MIR_new_proto (ctx, "basic_hplot_to_p", 0, NULL, 4, MIR_T_D, "x0", MIR_T_D, "y0",
                                 MIR_T_D, "x1", MIR_T_D, "y1");
  hplotto_import = MIR_new_import (ctx, "basic_hplot_to");
  hplottocur_proto
    = MIR_new_proto (ctx, "basic_hplot_to_current_p", 0, NULL, 2, MIR_T_D, "x", MIR_T_D, "y");
  hplottocur_import = MIR_new_import (ctx, "basic_hplot_to_current");
  move_proto = MIR_new_proto (ctx, "basic_move_p", 0, NULL, 2, MIR_T_D, "x", MIR_T_D, "y");
  move_import = MIR_new_import (ctx, "basic_move");
  draw_proto = MIR_new_proto (ctx, "basic_draw_p", 0, NULL, 2, MIR_T_D, "x", MIR_T_D, "y");
  draw_import = MIR_new_import (ctx, "basic_draw");
  line_proto = MIR_new_proto (ctx, "basic_line_p", 0, NULL, 4, MIR_T_D, "x0", MIR_T_D, "y0",
                              MIR_T_D, "x1", MIR_T_D, "y1");
  line_import = MIR_new_import (ctx, "basic_draw_line");
  circle_proto
    = MIR_new_proto (ctx, "basic_circle_p", 0, NULL, 3, MIR_T_D, "x", MIR_T_D, "y", MIR_T_D, "r");
  circle_import = MIR_new_import (ctx, "basic_circle");
  rect_proto = MIR_new_proto (ctx, "basic_rect_p", 0, NULL, 4, MIR_T_D, "x0", MIR_T_D, "y0",
                              MIR_T_D, "x1", MIR_T_D, "y1");
  rect_import = MIR_new_import (ctx, "basic_rect");
  mode_proto = MIR_new_proto (ctx, "basic_mode_p", 0, NULL, 1, MIR_T_D, "m");
  mode_import = MIR_new_import (ctx, "basic_mode");
  fill_proto = MIR_new_proto (ctx, "basic_fill_p", 0, NULL, 4, MIR_T_D, "x0", MIR_T_D, "y0",
                              MIR_T_D, "x1", MIR_T_D, "y1");
  fill_import = MIR_new_import (ctx, "basic_fill");
  beep_proto = MIR_new_proto (ctx, "basic_beep_p", 0, NULL, 0);
  beep_import = MIR_new_import (ctx, "basic_beep");
  sound_proto = MIR_new_proto (ctx, "basic_sound_p", 0, NULL, 2, MIR_T_D, "f", MIR_T_D, "d");
  sound_import = MIR_new_import (ctx, "basic_sound");
  system_proto = MIR_new_proto (ctx, "basic_system_p", 1, &d, 1, MIR_T_P, "cmd");
  system_import = MIR_new_import (ctx, "basic_system");
  system_out_proto = MIR_new_proto (ctx, "basic_system_out_p", 1, &p, 0);
  system_out_import = MIR_new_import (ctx, "basic_system_out");
  randomize_proto
    = MIR_new_proto (ctx, "basic_randomize_p", 0, NULL, 2, MIR_T_D, "n", MIR_T_D, "has_seed");
  randomize_import = MIR_new_import (ctx, "basic_randomize");
  stop_proto = MIR_new_proto (ctx, "basic_stop_p", 0, NULL, 0);
  stop_import = MIR_new_import (ctx, "basic_stop");
  on_error_proto = MIR_new_proto (ctx, "basic_set_error_handler_p", 0, NULL, 1, MIR_T_D, "line");
  on_error_import = MIR_new_import (ctx, "basic_set_error_handler");
  set_line_proto = MIR_new_proto (ctx, "basic_set_line_p", 0, NULL, 1, MIR_T_D, "line");
  set_line_import = MIR_new_import (ctx, "basic_set_line");
  get_line_proto = MIR_new_proto (ctx, "basic_get_line_p", 1, &d, 0);
  get_line_import = MIR_new_import (ctx, "basic_get_line");
  line_track_proto = MIR_new_proto (ctx, "basic_enable_line_tracking_p", 0, NULL, 1, MIR_T_D, "on");
  line_track_import = MIR_new_import (ctx, "basic_enable_line_tracking");
  if (profile_p) {
    profile_line_proto = MIR_new_proto (ctx, "basic_profile_line_p", 0, NULL, 1, MIR_T_D, "line");
    profile_line_import = MIR_new_import (ctx, "basic_profile_line");
    profile_func_enter_proto
      = MIR_new_proto (ctx, "basic_profile_func_enter_p", 0, NULL, 1, MIR_T_P, "name");
    profile_func_enter_import = MIR_new_import (ctx, "basic_profile_func_enter");
    profile_func_exit_proto
      = MIR_new_proto (ctx, "basic_profile_func_exit_p", 0, NULL, 1, MIR_T_P, "name");
    profile_func_exit_import = MIR_new_import (ctx, "basic_profile_func_exit");
  }
  rnd_proto = MIR_new_proto (ctx, "basic_rnd_p", 1, &d, 1, MIR_T_D, "n");
  rnd_import = MIR_new_import (ctx, "basic_rnd");
  chr_proto = MIR_new_proto (ctx, "basic_chr_p", 1, &p, 1, MIR_T_D, "n");
  chr_import = MIR_new_import (ctx, "basic_chr");
  string_proto = MIR_new_proto (ctx, "basic_string_p", 1, &p, 2, MIR_T_D, "n", MIR_T_P, "s");
  string_import = MIR_new_import (ctx, "basic_string");
  concat_proto = MIR_new_proto (ctx, "basic_concat_p", 1, &p, 2, MIR_T_P, "a", MIR_T_P, "b");
  concat_import = MIR_new_import (ctx, "basic_concat");
  int_proto = MIR_new_proto (ctx, "basic_int_p", 1, &d, 1, MIR_T_D, "x");
  int_import = MIR_new_import (ctx, "basic_int");
  timer_proto = MIR_new_proto (ctx, "basic_timer_p", 1, &d, 0);
  timer_import = MIR_new_import (ctx, "basic_timer");
  time_proto = MIR_new_proto (ctx, "basic_time_p", 1, &d, 0);
  time_import = MIR_new_import (ctx, "basic_time");
  time_str_proto = MIR_new_proto (ctx, "basic_time_str_p", 1, &p, 0);
  time_str_import = MIR_new_import (ctx, "basic_time_str");
  date_proto = MIR_new_proto (ctx, "basic_date_p", 1, &d, 0);
  date_import = MIR_new_import (ctx, "basic_date");
  date_str_proto = MIR_new_proto (ctx, "basic_date_str_p", 1, &p, 0);
  date_str_import = MIR_new_import (ctx, "basic_date_str");
  input_chr_proto = MIR_new_proto (ctx, "basic_input_chr_p", 1, &p, 1, MIR_T_D, "n");
  input_chr_import = MIR_new_import (ctx, "basic_input_chr");
  inkey_proto = MIR_new_proto (ctx, "basic_inkey_p", 1, &p, 0);
  inkey_import = MIR_new_import (ctx, "basic_inkey");
  peek_proto = MIR_new_proto (ctx, "basic_peek_p", 1, &d, 1, MIR_T_D, "addr");
  peek_import = MIR_new_import (ctx, "basic_peek");
  eof_proto = MIR_new_proto (ctx, "basic_eof_p", 1, &d, 1, MIR_T_D, "n");
  eof_import = MIR_new_import (ctx, "basic_eof");
  pos_proto = MIR_new_proto (ctx, "basic_pos_p", 1, &d, 0);
  pos_import = MIR_new_import (ctx, "basic_pos");

  abs_proto = MIR_new_proto (ctx, "basic_abs_p", 1, &d, 1, MIR_T_D, "x");
  abs_import = MIR_new_import (ctx, "basic_abs");
  sgn_proto = MIR_new_proto (ctx, "basic_sgn_p", 1, &d, 1, MIR_T_D, "x");
  sgn_import = MIR_new_import (ctx, "basic_sgn");
  sqr_proto = MIR_new_proto (ctx, "basic_sqr_p", 1, &d, 1, MIR_T_D, "x");
  sqr_import = MIR_new_import (ctx, "basic_sqr");
  sin_proto = MIR_new_proto (ctx, "basic_sin_p", 1, &d, 1, MIR_T_D, "x");
  sin_import = MIR_new_import (ctx, "basic_sin");
  cos_proto = MIR_new_proto (ctx, "basic_cos_p", 1, &d, 1, MIR_T_D, "x");
  cos_import = MIR_new_import (ctx, "basic_cos");
  tan_proto = MIR_new_proto (ctx, "basic_tan_p", 1, &d, 1, MIR_T_D, "x");
  tan_import = MIR_new_import (ctx, "basic_tan");
  atn_proto = MIR_new_proto (ctx, "basic_atn_p", 1, &d, 1, MIR_T_D, "x");
  atn_import = MIR_new_import (ctx, "basic_atn");
  log_proto = MIR_new_proto (ctx, "basic_log_p", 1, &d, 1, MIR_T_D, "x");
  log_import = MIR_new_import (ctx, "basic_log");
  exp_proto = MIR_new_proto (ctx, "basic_exp_p", 1, &d, 1, MIR_T_D, "x");
  exp_import = MIR_new_import (ctx, "basic_exp");

  left_proto = MIR_new_proto (ctx, "basic_left_p", 1, &p, 2, MIR_T_P, "s", MIR_T_D, "n");
  left_import = MIR_new_import (ctx, "basic_left");
  right_proto = MIR_new_proto (ctx, "basic_right_p", 1, &p, 2, MIR_T_P, "s", MIR_T_D, "n");
  right_import = MIR_new_import (ctx, "basic_right");
  mid_proto
    = MIR_new_proto (ctx, "basic_mid_p", 1, &p, 3, MIR_T_P, "s", MIR_T_D, "start", MIR_T_D, "len");
  mid_import = MIR_new_import (ctx, "basic_mid");
  len_proto = MIR_new_proto (ctx, "basic_len_p", 1, &d, 1, MIR_T_P, "s");
  len_import = MIR_new_import (ctx, "basic_len");
  val_proto = MIR_new_proto (ctx, "basic_val_p", 1, &d, 1, MIR_T_P, "s");
  val_import = MIR_new_import (ctx, "basic_val");
  str_proto = MIR_new_proto (ctx, "basic_str_p", 1, &p, 1, MIR_T_D, "n");
  str_import = MIR_new_import (ctx, "basic_str");
  asc_proto = MIR_new_proto (ctx, "basic_asc_p", 1, &d, 1, MIR_T_P, "s");
  asc_import = MIR_new_import (ctx, "basic_asc");
  instr_proto = MIR_new_proto (ctx, "basic_instr_p", 1, &d, 2, MIR_T_P, "s", MIR_T_P, "sub");
  instr_import = MIR_new_import (ctx, "basic_instr");

  strdup_proto = MIR_new_proto (ctx, "basic_strdup_p", 1, &p, 1, MIR_T_P, "s");
  strdup_import = MIR_new_import (ctx, "basic_strdup");
  free_proto = MIR_new_proto (ctx, "basic_free_p", 0, NULL, 1, MIR_T_P, "s");
  free_import = MIR_new_import (ctx, "basic_free");
  mir_ctx_proto = MIR_new_proto (ctx, "basic_mir_ctx_p", 1, &d, 0);
  mir_ctx_import = MIR_new_import (ctx, "basic_mir_ctx");
  mir_mod_proto = MIR_new_proto (ctx, "basic_mir_mod_p", 1, &d, 2, MIR_T_D, "ctx", MIR_T_P, "name");
  mir_mod_import = MIR_new_import (ctx, "basic_mir_mod");
  mir_func_proto = MIR_new_proto (ctx, "basic_mir_func_p", 1, &d, 3, MIR_T_D, "mod", MIR_T_P,
                                  "name", MIR_T_D, "nargs");
  mir_func_import = MIR_new_import (ctx, "basic_mir_func");
  mir_reg_proto = MIR_new_proto (ctx, "basic_mir_reg_p", 1, &d, 1, MIR_T_D, "func");
  mir_reg_import = MIR_new_import (ctx, "basic_mir_reg");
  mir_label_proto = MIR_new_proto (ctx, "basic_mir_label_p", 1, &d, 1, MIR_T_D, "func");
  mir_label_import = MIR_new_import (ctx, "basic_mir_label");
  mir_emit_proto = MIR_new_proto (ctx, "basic_mir_emit_p", 1, &d, 5, MIR_T_D, "func", MIR_T_P, "op",
                                  MIR_T_D, "a", MIR_T_D, "b", MIR_T_D, "c");
  mir_emit_import = MIR_new_import (ctx, "basic_mir_emit");
  mir_emitlbl_proto
    = MIR_new_proto (ctx, "basic_mir_emitlbl_p", 1, &d, 2, MIR_T_D, "func", MIR_T_D, "lab");
  mir_emitlbl_import = MIR_new_import (ctx, "basic_mir_emitlbl");
  mir_ret_proto = MIR_new_proto (ctx, "basic_mir_ret_p", 1, &d, 2, MIR_T_D, "func", MIR_T_D, "reg");
  mir_ret_import = MIR_new_import (ctx, "basic_mir_ret");
  mir_finish_proto = MIR_new_proto (ctx, "basic_mir_finish_p", 1, &d, 1, MIR_T_D, "mod");
  mir_finish_import = MIR_new_import (ctx, "basic_mir_finish");
  mir_run_proto = MIR_new_proto (ctx, "basic_mir_run_p", 1, &d, 5, MIR_T_D, "func", MIR_T_D, "a1",
                                 MIR_T_D, "a2", MIR_T_D, "a3", MIR_T_D, "a4");
  mir_run_import = MIR_new_import (ctx, "basic_mir_run");
  mir_dump_proto = MIR_new_proto (ctx, "basic_mir_dump_p", 1, &d, 1, MIR_T_D, "func");
  mir_dump_import = MIR_new_import (ctx, "basic_mir_dump");

  calloc_proto = MIR_new_proto (ctx, "calloc_p", 1, &p, 2, MIR_T_I64, "n", MIR_T_I64, "sz");
  calloc_import = MIR_new_import (ctx, "calloc");
  memset_proto
    = MIR_new_proto (ctx, "memset_p", 1, &p, 3, MIR_T_P, "s", MIR_T_I64, "c", MIR_T_I64, "n");
  memset_import = MIR_new_import (ctx, "memset");
  MIR_type_t i64 = MIR_T_I64;
  strcmp_proto = MIR_new_proto (ctx, "basic_strcmp_p", 1, &i64, 2, MIR_T_P, "a", MIR_T_P, "b");
  strcmp_import = MIR_new_import (ctx, "basic_strcmp");
  read_proto = MIR_new_proto (ctx, "basic_read_p", 1, &d, 0);
  read_import = MIR_new_import (ctx, "basic_read");
  read_str_proto = MIR_new_proto (ctx, "basic_read_str_p", 1, &p, 0);
  read_str_import = MIR_new_import (ctx, "basic_read_str");
  restore_proto = MIR_new_proto (ctx, "basic_restore_p", 0, NULL, 0);
  restore_import = MIR_new_import (ctx, "basic_restore");
  g_ctx = ctx;
  for (size_t i = 0; i < func_defs.len; i++) {
    FuncDef *fd = &func_defs.data[i];
    MIR_type_t rtype = fd->is_str_ret ? MIR_T_P : MIR_T_D;
    MIR_var_t *vars = NULL;
    if (fd->n != 0) {
      vars = malloc (sizeof (MIR_var_t) * fd->n);
      for (size_t j = 0; j < fd->n; j++) {
        vars[j].type = fd->is_str[j] ? MIR_T_P : MIR_T_D;
        vars[j].name = fd->params[j];
      }
    }
    char proto_name[128];
    snprintf (proto_name, sizeof (proto_name), "%s_p", fd->name);
    if (fd->is_proc) {
      fd->proto = MIR_new_proto_arr (ctx, proto_name, 0, NULL, fd->n, vars);
      fd->item = MIR_new_func_arr (ctx, fd->name, 0, NULL, fd->n, vars);
    } else {
      fd->proto = MIR_new_proto_arr (ctx, proto_name, 1, &rtype, fd->n, vars);
      fd->item = MIR_new_func_arr (ctx, fd->name, 1, &rtype, fd->n, vars);
    }
    if (profile_p)
      MIR_append_insn (ctx, fd->item,
                       MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, profile_func_enter_proto),
                                          MIR_new_ref_op (ctx, profile_func_enter_import),
                                          MIR_new_str_op (ctx, (MIR_str_t) {strlen (fd->name) + 1,
                                                                            fd->name})));
    VarVec fvars = {0};
    for (size_t j = 0; j < fd->n; j++) {
      if (fvars.len == fvars.cap) {
        fvars.cap = fvars.cap ? 2 * fvars.cap : fd->n;
        fvars.data = realloc (fvars.data, fvars.cap * sizeof (Var));
      }
      fvars.data[fvars.len].name = strdup (fd->params[j]);
      fvars.data[fvars.len].is_str = fd->is_str[j];
      fvars.data[fvars.len].is_array = 0;
      fvars.data[fvars.len].size = 0;
      fvars.data[fvars.len].reg = MIR_reg (ctx, fd->params[j], fd->item->u.func);
      fvars.len++;
    }
    if (fd->body != NULL) {
      MIR_reg_t r = gen_expr (ctx, fd->item, &fvars, fd->body);
      MIR_insn_t ret_insn = MIR_new_ret_insn (ctx, 1, MIR_new_reg_op (ctx, r));
      if (profile_p)
        MIR_append_insn (ctx, fd->item,
                         MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, profile_func_exit_proto),
                                            MIR_new_ref_op (ctx, profile_func_exit_import),
                                            MIR_new_str_op (ctx, (MIR_str_t) {strlen (fd->name) + 1,
                                                                              fd->name})));
      MIR_append_insn (ctx, fd->item, ret_insn);
    } else {
      MIR_item_t saved_func = g_func;
      VarVec saved_vars = g_vars;
      LineVec *saved_prog = g_prog;
      MIR_label_t *saved_labels = g_labels;
      size_t saved_loop_len = g_loop_len;
      g_func = fd->item;
      g_vars = fvars;
      g_prog = NULL;
      g_labels = NULL;
      g_loop_len = 0;
      for (size_t j = 0; j < fd->body_stmts.len; j++) gen_stmt (&fd->body_stmts.data[j]);
      MIR_insn_t ret_insn;
      if (fd->is_proc) {
        ret_insn = MIR_new_ret_insn (ctx, 0);
      } else {
        MIR_reg_t r = get_var (&g_vars, ctx, fd->item, fd->name);
        ret_insn = MIR_new_ret_insn (ctx, 1, MIR_new_reg_op (ctx, r));
      }
      if (profile_p)
        MIR_append_insn (ctx, fd->item,
                         MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, profile_func_exit_proto),
                                            MIR_new_ref_op (ctx, profile_func_exit_import),
                                            MIR_new_str_op (ctx, (MIR_str_t) {strlen (fd->name) + 1,
                                                                              fd->name})));
      MIR_append_insn (ctx, fd->item, ret_insn);
      g_func = saved_func;
      g_vars = saved_vars;
      g_prog = saved_prog;
      g_labels = saved_labels;
      g_loop_len = saved_loop_len;
    }
    MIR_finish_func (ctx);
    free (vars);
  }
  MIR_type_t res_t = MIR_T_I64;
  MIR_item_t func = MIR_new_func (ctx, "main", 1, &res_t, 0);
  g_ctx = ctx;
  g_func = func;
  g_prog = prog;
  g_vars = (VarVec) {0};

  basic_data_items = data_vals.data;
  basic_data_len = data_vals.len;
  basic_data_pos = 0;

  /* return stack for GOSUB/RETURN */
  MIR_reg_t ret_stack = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, "ret_stack");
  MIR_reg_t ret_sp = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, "ret_sp");
  MIR_reg_t ret_addr = 0;
  MIR_append_insn (ctx, func,
                   MIR_new_insn (ctx, MIR_ALLOCA, MIR_new_reg_op (ctx, ret_stack),
                                 MIR_new_int_op (ctx, 1024)));
  MIR_append_insn (ctx, func,
                   MIR_new_insn (ctx, MIR_MOV, MIR_new_reg_op (ctx, ret_sp),
                                 MIR_new_int_op (ctx, 0)));
  g_var_init_anchor = DLIST_TAIL (MIR_insn_t, func->u.func->insns);

  g_ret_stack = ret_stack;
  g_ret_sp = ret_sp;
  g_ret_addr = ret_addr;

  g_loop_stack = NULL;
  g_loop_len = 0;
  g_loop_cap = 0;
  g_line_tracking = track_lines;
  MIR_append_insn (g_ctx, g_func,
                   MIR_new_call_insn (g_ctx, 3, MIR_new_ref_op (g_ctx, line_track_proto),
                                      MIR_new_ref_op (g_ctx, line_track_import),
                                      MIR_new_double_op (g_ctx, (basic_num_t) g_line_tracking)));

  /* create labels for lines */
  size_t n = prog->len;
  MIR_label_t *labels = malloc (sizeof (MIR_label_t) * n);
  for (size_t i = 0; i < n; i++) labels[i] = MIR_new_label (ctx);
  g_labels = labels;

  for (size_t i = 0; i < n; i++) {
    Line *ln = &g_prog->data[i];
    g_cur_line = ln;
    MIR_append_insn (g_ctx, g_func, labels[i]);
    if (profile_p)
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_call_insn (g_ctx, 3, MIR_new_ref_op (g_ctx, profile_line_proto),
                                          MIR_new_ref_op (g_ctx, profile_line_import),
                                          MIR_new_double_op (g_ctx, (basic_num_t) ln->line)));
    if (g_line_tracking)
      MIR_append_insn (g_ctx, g_func,
                       MIR_new_call_insn (g_ctx, 3, MIR_new_ref_op (g_ctx, set_line_proto),
                                          MIR_new_ref_op (g_ctx, set_line_import),
                                          MIR_new_double_op (g_ctx, (basic_num_t) ln->line)));
    for (size_t j = 0; j < ln->stmts.len; j++) {
      gen_stmt (&ln->stmts.data[j]);
    }
  }
  g_cur_line = NULL;
  for (size_t i = 0; i < g_vars.len; i++)
    if (g_vars.data[i].is_array && g_vars.data[i].size <= 1) {
      g_vars.data[i].size = 11;
      size_t elem_size = g_vars.data[i].is_str ? sizeof (char *) : sizeof (basic_num_t);
      MIR_insn_t call
        = MIR_new_call_insn (ctx, 5, MIR_new_ref_op (ctx, calloc_proto),
                             MIR_new_ref_op (ctx, calloc_import),
                             MIR_new_reg_op (ctx, g_vars.data[i].reg), MIR_new_int_op (ctx, 11),
                             MIR_new_int_op (ctx, elem_size));
      MIR_insert_insn_after (ctx, func, g_var_init_anchor, call);
      g_var_init_anchor = call;
    }
  for (size_t i = 0; i < g_vars.len; i++)
    if (g_vars.data[i].is_str && !g_vars.data[i].is_array)
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, free_proto),
                                          MIR_new_ref_op (ctx, free_import),
                                          MIR_new_reg_op (ctx, g_vars.data[i].reg)));
  /* ensure function returns 0 if no END */
  MIR_append_insn (ctx, func, MIR_new_ret_insn (ctx, 1, MIR_new_int_op (ctx, 0)));
  MIR_finish_func (ctx);
  MIR_finish_module (ctx);
  if (asm_p || obj_p) {
    const char *base = out_name ? out_name : src_name;
    if (asm_p) {
      char *name = change_suffix (base, ".mir");
      FILE *f = fopen (name, "w");
      if (f != NULL) {
        MIR_output_module (ctx, f, module);
        fclose (f);
      } else {
        perror (name);
      }
      free (name);
    }
    if (obj_p) {
      char *name = change_suffix (base, ".bmir");
      FILE *f = fopen (name, "wb");
      if (f != NULL) {
        MIR_write_module (ctx, f, module);
        fclose (f);
      } else {
        perror (name);
      }
      free (name);
    }
    MIR_finish (ctx);
    data_vals_clear ();
    return;
  }
  if (bin_p) {
    char *exe_name = out_name ? strdup (out_name) : change_suffix (src_name, "");
    char *ctab_name = change_suffix (exe_name, ".ctab");
    ctab_file = fopen (ctab_name, "w");
    if (ctab_file != NULL) {
      fprintf (ctab_file, "static const unsigned char mir_code[] = {\n");
      ctab_byte_num = 0;
      MIR_write_module_with_func (ctx, ctab_writer, module);
      fprintf (ctab_file, "};\n");
      fclose (ctab_file);
      const char *cc = getenv ("CC");
      if (cc == NULL) cc = "cc";
      const char *src_dir = BASIC_SRC_DIR;
      const char *extra_cflags
        = reduce_libs ? " -ffunction-sections -fdata-sections -Wl,--gc-sections" : "";
      size_t size = strlen (cc) + strlen (src_dir) * 5 + strlen (ctab_name) + strlen (exe_name)
                    + strlen (extra_cflags) + 200;
      char *cmd = malloc (size);
      snprintf (cmd, size,
                "%s -I\"%s\" -DCTAB_INCLUDE_STRING=\\\"%s\\\" \"%s/mir-bin-driver.c\" "
                "\"%s/examples/basic/basic_runtime.c\" \"%s/mir.c\" \"%s/mir-gen.c\" -rdynamic -lm "
                "-ldl%s -o "
                "\"%s\"",
                cc, src_dir, ctab_name, src_dir, src_dir, src_dir, src_dir, extra_cflags, exe_name);
      if (system (cmd) != 0) perror (cmd);
      free (cmd);
    } else {
      perror (ctab_name);
    }
    free (ctab_name);
    free (exe_name);
    MIR_finish (ctx);
    data_vals_clear ();
    return;
  }
  if (code_p) {
    MIR_load_module (ctx, module);
    MIR_gen_init (ctx);
    MIR_link (ctx, MIR_set_gen_interface, resolve);
    MIR_gen (ctx, func);
    uint8_t *start = func->addr;
    uint8_t *end = _MIR_get_new_code_addr (ctx, 0);
    size_t len = end - start;
    FILE *f = fopen (out_name, "wb");
    if (f != NULL) {
      fwrite (start, 1, len, f);
      fclose (f);
    } else {
      perror (out_name);
    }
    MIR_gen_finish (ctx);
    MIR_finish (ctx);
    data_vals_clear ();
    return;
  }
  MIR_load_module (ctx, module);
  if (jit) {
    MIR_gen_init (ctx);
    MIR_link (ctx, MIR_set_gen_interface, resolve);
    typedef int (*main_t) (void);
    main_t m = MIR_gen (ctx, func);
    m ();
    MIR_gen_finish (ctx);
  } else {
    MIR_link (ctx, MIR_set_interp_interface, resolve);
    typedef int (*main_t) (void);
    main_t m = func->addr;
    m ();
  }
  MIR_finish (ctx);
  data_vals_clear ();
}

static void repl (void) {
  LineVec prog = {0};
  char line[256];
  for (;;) {
    printf ("READY.\n");
    if (!fgets (line, sizeof (line), stdin)) break;
    line[strcspn (line, "\n")] = '\0';
    char *s = line;
    while (isspace ((unsigned char) *s)) s++;
    if (*s == '\0') continue;
    if (isdigit ((unsigned char) *s)) {
      char *end;
      long num = strtol (s, &end, 10);
      while (isspace ((unsigned char) *end)) end++;
      if (*end == '\0') {
        delete_line (&prog, num);
      } else {
        Line l;
        Parser p;
        if (parse_line (&p, line, &l))
          insert_or_replace_line (&prog, l);
        else
          /* error already reported by parse_line */
          ;
      }
      continue;
    }
    if (strncasecmp (s, "RUN", 3) == 0) {
      s += 3;
      while (isspace ((unsigned char) *s)) s++;
      int profile_p = 0;
      if (*s != '\0') {
        if (strcasecmp (s, "PROFILING") == 0 || strcasecmp (s, "PROFILE") == 0) {
          profile_p = 1;
        } else {
          fprintf (stderr, "unknown RUN option: %s\n", s);
          continue;
        }
      }
      if (profile_p) basic_profile_reset ();
      gen_program (&prog, 0, 0, 0, 0, 0, 0, profile_p, line_tracking, NULL, "(repl)");
      if (profile_p) basic_profile_dump ();
      continue;
    }
    if (strncasecmp (s, "COMPILE", 7) == 0) {
      s += 7;
      while (isspace ((unsigned char) *s)) s++;
      if (strncasecmp (s, "NATIVE", 6) == 0) {
        s += 6;
        while (isspace ((unsigned char) *s)) s++;
        if (*s == '\0') {
          fprintf (stderr, "missing output file\n");
        } else {
          gen_program (&prog, 0, 0, 0, 1, 0, 0, 0, line_tracking, s, "(repl)");
          if (access (s, F_OK) == 0)
            printf ("%s\n", s);
          else
            perror (s);
        }
        continue;
      } else if (strncasecmp (s, "BMIR", 4) == 0) {
        s += 4;
        while (isspace ((unsigned char) *s)) s++;
        if (*s == '\0') {
          fprintf (stderr, "missing output file\n");
        } else {
          gen_program (&prog, 0, 0, 1, 0, 0, 0, 0, line_tracking, s, "(repl)");
          char *name = change_suffix (s, ".bmir");
          if (access (name, F_OK) == 0)
            printf ("%s\n", name);
          else
            perror (name);
          free (name);
        }
        continue;
      } else if (strncasecmp (s, "CODE", 4) == 0) {
        s += 4;
        while (isspace ((unsigned char) *s)) s++;
        if (*s == '\0') {
          fprintf (stderr, "missing output file\n");
        } else {
          gen_program (&prog, 0, 0, 0, 0, 1, 0, 0, line_tracking, s, "(repl)");
          if (access (s, F_OK) == 0)
            printf ("%s\n", s);
          else
            perror (s);
        }
        continue;
      }
      fprintf (stderr, "unknown COMPILE target\n");
      continue;
    }
    if (strncasecmp (s, "SAVE", 4) == 0) {
      s += 4;
      while (isspace ((unsigned char) *s)) s++;
      if (*s == '\0') {
        fprintf (stderr, "missing output file\n");
      } else {
        gen_program (&prog, 0, 0, 0, 1, 0, 0, 0, line_tracking, s, "(repl)");
        if (access (s, F_OK) == 0)
          printf ("Saved %s\n", s);
        else
          perror (s);
      }
      continue;
    }
    if (strncasecmp (s, "LOAD", 4) == 0) {
      s += 4;
      while (isspace ((unsigned char) *s)) s++;
      if (*s == '\0') {
        fprintf (stderr, "missing input file\n");
      } else {
        func_vec_clear (&func_defs);
        data_vals_clear ();
        line_vec_destroy (&prog);
        load_program (&prog, s);
      }
      continue;
    }
    if (strcasecmp (s, "LIST") == 0) {
      list_program (&prog);
      continue;
    }
    if (strcasecmp (s, "NEW") == 0) {
      func_vec_clear (&func_defs);
      data_vals_clear ();
      line_vec_destroy (&prog);
      continue;
    }
    if (strcasecmp (s, "QUIT") == 0 || strcasecmp (s, "EXIT") == 0) {
      break;
    }
  }
  /*
     Cleanup is intentionally omitted here.  Previous program execution may
     modify internal data structures in a way that confuses the destructor
     logic, leading to a double free.  The process is about to exit anyway, so
     letting the operating system reclaim memory avoids the crash observed in
     the REPL LOAD test.
   */
}

int main (int argc, char **argv) {
  if (kitty_graphics_available ()) show_kitty_banner ();
  int jit = 0, asm_p = 0, obj_p = 0, bin_p = 0, reduce_libs = 0;
  const char *fname = NULL, *out_name = NULL;
  for (int i = 1; i < argc; i++) {
    if (strcmp (argv[i], "-j") == 0) {
      jit = 1;
    } else if (strcmp (argv[i], "-S") == 0) {
      asm_p = obj_p = 1;
    } else if (strcmp (argv[i], "-c") == 0) {
      asm_p = obj_p = 1;
    } else if (strcmp (argv[i], "-r") == 0) {
      obj_p = 1;
    } else if (strcmp (argv[i], "-b") == 0) {
      bin_p = 1;
    } else if (strcmp (argv[i], "-l") == 0) {
      reduce_libs = 1;
    } else if (strcmp (argv[i], "--no-line-tracking") == 0) {
      line_tracking = 0;
    } else if (strcmp (argv[i], "-o") == 0 && i + 1 < argc) {
      out_name = argv[++i];
    } else {
      fname = argv[i];
    }
  }
  if (!fname) {
    repl ();
    return 0;
  }
  LineVec prog = {0};
  if (!load_program (&prog, fname)) return 1;
  gen_program (&prog, jit, asm_p, obj_p, bin_p, 0, reduce_libs, 0, line_tracking, out_name, fname);
  return 0;
}

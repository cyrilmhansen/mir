#include "mir.h"
#include "mir-gen.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>

#ifndef BASIC_SRC_DIR
#define BASIC_SRC_DIR "."
#endif

/* Runtime helpers defined in basic_runtime.c */
extern void basic_print (double);
extern void basic_print_str (const char *);
extern double basic_input (void);
extern char *basic_input_str (void);
extern char *basic_get (void);
extern int basic_strcmp (const char *, const char *);

extern double basic_read (void);
extern char *basic_read_str (void);
extern void basic_restore (void);

typedef struct BasicData {
  int is_str;
  double num;
  char *str;
} BasicData;

extern BasicData *basic_data_items;
extern size_t basic_data_len;
extern size_t basic_data_pos;

extern void basic_home (void);
extern void basic_vtab (double);
extern double basic_rnd (double);

extern void basic_screen (double);
extern void basic_cls (void);
extern void basic_color (double);
extern void basic_key_off (void);
extern void basic_locate (double, double);
extern void basic_htab (double);

extern char *basic_chr (double);
extern char *basic_string (double, const char *);
extern double basic_int (double);
extern double basic_timer (void);
extern char *basic_input_chr (double);
extern double basic_peek (double);
extern void basic_poke (double, double);

static void *resolve (const char *name) {
  if (!strcmp (name, "basic_print")) return basic_print;
  if (!strcmp (name, "basic_print_str")) return basic_print_str;
  if (!strcmp (name, "basic_input")) return basic_input;
  if (!strcmp (name, "basic_input_str")) return basic_input_str;
  if (!strcmp (name, "basic_get")) return basic_get;
  if (!strcmp (name, "basic_strcmp")) return basic_strcmp;

  if (!strcmp (name, "basic_read")) return basic_read;
  if (!strcmp (name, "basic_read_str")) return basic_read_str;
  if (!strcmp (name, "basic_restore")) return basic_restore;

  if (!strcmp (name, "basic_home")) return basic_home;
  if (!strcmp (name, "basic_vtab")) return basic_vtab;
  if (!strcmp (name, "basic_rnd")) return basic_rnd;

  if (!strcmp (name, "basic_screen")) return basic_screen;
  if (!strcmp (name, "basic_cls")) return basic_cls;
  if (!strcmp (name, "basic_color")) return basic_color;
  if (!strcmp (name, "basic_key_off")) return basic_key_off;
  if (!strcmp (name, "basic_locate")) return basic_locate;
  if (!strcmp (name, "basic_htab")) return basic_htab;
  if (!strcmp (name, "basic_tab")) return basic_htab;

  if (!strcmp (name, "basic_chr")) return basic_chr;
  if (!strcmp (name, "basic_string")) return basic_string;
  if (!strcmp (name, "basic_int")) return basic_int;
  if (!strcmp (name, "basic_timer")) return basic_timer;
  if (!strcmp (name, "basic_input_chr")) return basic_input_chr;
  if (!strcmp (name, "basic_peek")) return basic_peek;
  if (!strcmp (name, "basic_poke")) return basic_poke;

  if (!strcmp (name, "calloc")) return calloc;
  if (!strcmp (name, "memset")) return memset;
  return NULL;
}

/* Runtime call prototypes for expressions */
static MIR_item_t rnd_proto, rnd_import, chr_proto, chr_import, string_proto, string_import,
  int_proto, int_import, timer_proto, timer_import, input_chr_proto, input_chr_import, peek_proto,
  peek_import;

/* Runtime call prototypes for statements */
static MIR_item_t print_proto, print_import, prints_proto, prints_import, input_proto, input_import,
  input_str_proto, input_str_import, get_proto, get_import, read_proto, read_import, read_str_proto,
  read_str_import, restore_proto, restore_import, screen_proto, screen_import, cls_proto,
  cls_import, color_proto, color_import, keyoff_proto, keyoff_import, locate_proto, locate_import,
  htab_proto, htab_import, home_proto, poke_proto, poke_import, home_import, vtab_proto, vtab_import, calloc_proto,
  calloc_import, memset_proto, memset_import, strcmp_proto, strcmp_import;

/* AST for expressions */
typedef enum { N_NUM, N_VAR, N_BIN, N_NEG, N_STR, N_CALL } NodeKind;
typedef struct Node Node;
struct Node {
  NodeKind kind;
  int is_str;
  double num;
  char *var;
  char *str;
  char op;
  Node *left, *right;
  Node *index;
};

static Node *new_node (NodeKind k) {
  Node *n = malloc (sizeof (Node));
  n->kind = k;
  n->is_str = 0;
  n->num = 0;
  n->var = NULL;
  n->str = NULL;
  n->op = 0;
  n->left = n->right = n->index = NULL;
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
  ST_END,
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
} StmtKind;
typedef enum { REL_NONE, REL_EQ, REL_NE, REL_LT, REL_LE, REL_GT, REL_GE } Relop;
typedef struct Stmt Stmt;
struct Stmt {
  StmtKind kind;
  union {
    Node *expr; /* PRINT/VTAB/HTAB/SCREEN/COLOR/WHILE */
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
      Stmt *stmt;
    } iff;
    struct {
      char *var;
      int is_str;
    } input;
    struct {
      char *var;
    } get;
    struct {
      Node **vars;
      size_t n;
    } read;
    struct {
      char **names;
      int *sizes;
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
      Node *addr;
      Node *value;
    } poke;
    int target; /* GOTO/GOSUB */
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
  } u;
};

/* Dynamic array of statements */
typedef struct {
  Stmt *data;
  size_t len, cap;
} StmtVec;

/* Program line containing multiple statements */
typedef struct {
  int line;
  StmtVec stmts;
} Line;

typedef struct {
  Line *data;
  size_t len, cap;
} LineVec;

typedef struct {
  MIR_reg_t var, end, step;
  MIR_label_t start_label, end_label;
} LoopInfo;

static void vec_push (StmtVec *v, Stmt s) {
  if (v->len == v->cap) {
    v->cap = v->cap ? 2 * v->cap : 16;
    v->data = realloc (v->data, v->cap * sizeof (Stmt));
  }
  v->data[v->len++] = s;
}

static void line_vec_push (LineVec *v, Line l) {
  if (v->len == v->cap) {
    v->cap = v->cap ? 2 * v->cap : 16;
    v->data = realloc (v->data, v->cap * sizeof (Line));
  }
  v->data[v->len++] = l;
}

/* Parsing utilities */
static char *cur;
static void skip_ws (void) {
  while (*cur && isspace ((unsigned char) *cur)) cur++;
}
static int parse_int (void) {
  skip_ws ();
  int v = strtol (cur, &cur, 10);
  return v;
}
static char *parse_id (void) {
  skip_ws ();
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
  buf[i] = 0;
  return strdup (buf);
}
static double parse_number (void) {
  skip_ws ();
  double v = strtod (cur, &cur);
  return v;
}
static char *parse_string (void) {
  skip_ws ();
  if (*cur != '"') return NULL;
  cur++;
  const char *start = cur;
  while (*cur && *cur != '"') cur++;
  size_t len = cur - start;
  char *s = malloc (len + 1);
  memcpy (s, start, len);
  s[len] = 0;
  if (*cur == '"') cur++;
  return s;
}

/* Expression parser */
static Node *parse_factor (void);
static Node *parse_term (void);
static Node *parse_add (void);

static Node *parse_rel (void);
static Node *parse_logical (void);

static Node *parse_expr (void);
static Relop parse_relop (void);

static Node *parse_factor (void) {
  skip_ws ();
  if (*cur == '(') {
    cur++;
    Node *e = parse_expr ();
    skip_ws ();
    if (*cur == ')') cur++;
    return e;
  }
  if (*cur == '-') {
    cur++;
    Node *n = new_node (N_NEG);
    n->left = parse_factor ();
    return n;
  }
  if (isdigit ((unsigned char) *cur) || *cur == '.') {
    Node *n = new_node (N_NUM);
    n->num = parse_number ();
    return n;
  }
  if (*cur == '"') {
    Node *n = new_node (N_STR);
    n->is_str = 1;
    n->str = parse_string ();
    return n;
  }
  if (isalpha ((unsigned char) *cur)) {
    char *id = parse_id ();
    skip_ws ();
    Node *arg1 = NULL, *arg2 = NULL;
    if (*cur == '(') {
      cur++;
      skip_ws ();
      if (*cur != ')') {
        arg1 = parse_expr ();
        skip_ws ();
        if (*cur == ',') {
          cur++;
          arg2 = parse_expr ();
          skip_ws ();
        }
      }
      if (*cur == ')') cur++;
    }
    if (strcasecmp (id, "RND") == 0 || strcasecmp (id, "CHR$") == 0
        || strcasecmp (id, "STRING$") == 0 || strcasecmp (id, "INT") == 0
        || strcasecmp (id, "TIMER") == 0 || strcasecmp (id, "INPUT$") == 0
        || strcasecmp (id, "PEEK") == 0 || strcasecmp (id, "SPC") == 0)
     {
      Node *n = new_node (N_CALL);
      n->var = id;
      n->left = arg1;
      n->right = arg2;
      if (strcasecmp (id, "CHR$") == 0 || strcasecmp (id, "STRING$") == 0
          || strcasecmp (id, "INPUT$") == 0 || strcasecmp (id, "SPC") == 0)
        n->is_str = 1;
      return n;
    }
    if (arg1 != NULL) {
      Node *n = new_node (N_VAR);
      n->var = id;
      n->is_str = id[strlen (id) - 1] == '$';
      n->index = arg1;
      return n;
    } else {
      Node *n = new_node (N_VAR);
      n->var = id;
      n->is_str = id[strlen (id) - 1] == '$';
      return n;
    }
  }
  return NULL;
}

static Node *parse_term (void) {
  Node *n = parse_factor ();
  while (1) {
    skip_ws ();
    char op = *cur;
    if (op != '*' && op != '/') break;
    cur++;
    Node *r = parse_factor ();
    Node *nn = new_node (N_BIN);
    nn->op = op;
    nn->left = n;
    nn->right = r;
    n = nn;
  }
  return n;
}

static Node *parse_add (void) {
  Node *n = parse_term ();
  while (1) {
    skip_ws ();
    char op = *cur;
    if (op != '+' && op != '-') break;
    cur++;
    Node *r = parse_term ();
    Node *nn = new_node (N_BIN);
    nn->op = op;
    nn->left = n;
    nn->right = r;
    n = nn;
  }
  return n;
}

static Node *parse_rel (void) {
  Node *n = parse_add ();
  Relop r = parse_relop ();
  if (r != REL_NONE) {
    Node *rhs = parse_add ();
    Node *nn = new_node (N_BIN);
    switch (r) {
    case REL_EQ: nn->op = '='; break;
    case REL_NE: nn->op = '!'; break;
    case REL_LT: nn->op = '<'; break;
    case REL_LE: nn->op = 'L'; break;
    case REL_GT: nn->op = '>'; break;
    case REL_GE: nn->op = 'G'; break;
    default: break;
    }
    nn->left = n;
    nn->right = rhs;
    n = nn;
  }
  return n;
}

static Node *parse_and (void) {
  Node *n = parse_add ();
  while (1) {
    skip_ws ();
    if (strncasecmp (cur, "AND", 3) != 0 || isalnum ((unsigned char) cur[3]) || cur[3] == '_'
        || cur[3] == '$')
      break;
    cur += 3;
    Node *r = parse_add ();
    Node *nn = new_node (N_BIN);
    nn->op = '&';
    nn->left = n;
    nn->right = r;
    n = nn;
  }
  return n;
}

static Node *parse_logical (void) {
  Node *n = parse_rel ();
  while (1) {
    skip_ws ();
    if (strncasecmp (cur, "AND", 3) == 0) {
      cur += 3;
      Node *r = parse_rel ();
      Node *nn = new_node (N_BIN);
      nn->op = '&';
      nn->left = n;
      nn->right = r;
      n = nn;
      continue;
    }
    if (strncasecmp (cur, "OR", 2) == 0) {
      cur += 2;
      Node *r = parse_rel ();
      Node *nn = new_node (N_BIN);
      nn->op = '|';
      nn->left = n;
      nn->right = r;
      n = nn;
      continue;
    }
    break;
  }
  return n;
}

static Node *parse_expr (void) { return parse_logical (); }

static Relop parse_relop (void) {
  skip_ws ();
  if (*cur == '=') {
    cur++;
    return REL_EQ;
  }
  if (*cur == '<' && cur[1] == '>') {
    cur += 2;
    return REL_NE;
  }
  if (*cur == '<' && cur[1] == '=') {
    cur += 2;
    return REL_LE;
  }
  if (*cur == '>' && cur[1] == '=') {
    cur += 2;
    return REL_GE;
  }
  if (*cur == '<') {
    cur++;
    return REL_LT;
  }
  if (*cur == '>') {
    cur++;
    return REL_GT;
  }
  return REL_NONE;
}

/* Parse a single line into statement */
static int parse_stmt (Stmt *out) {
  skip_ws ();
  if (strncasecmp (cur, "REM", 3) == 0) {
    /* Comment line: ignore rest */
    out->kind = ST_REM;
    return 1;
  } else if (strncasecmp (cur, "DIM", 3) == 0) {
    cur += 3;
    out->kind = ST_DIM;
    out->u.dim.names = NULL;
    out->u.dim.sizes = NULL;
    out->u.dim.is_str = NULL;
    out->u.dim.n = 0;
    size_t cap = 0;
    while (1) {
      char *name = parse_id ();
      int is_str = name[strlen (name) - 1] == '$';
      skip_ws ();
      int size = 0;
      if (*cur == '(') {
        cur++;
        size = parse_int ();
        skip_ws ();
        if (*cur == ')') cur++;
      }
      if (out->u.dim.n == cap) {
        cap = cap ? 2 * cap : 4;
        out->u.dim.names = realloc (out->u.dim.names, cap * sizeof (char *));
        out->u.dim.sizes = realloc (out->u.dim.sizes, cap * sizeof (int));
        out->u.dim.is_str = realloc (out->u.dim.is_str, cap * sizeof (int));
      }
      out->u.dim.names[out->u.dim.n] = name;
      out->u.dim.sizes[out->u.dim.n] = size;
      out->u.dim.is_str[out->u.dim.n] = is_str;
      out->u.dim.n++;
      skip_ws ();
      if (*cur != ',') break;
      cur++;
    }
    return 1;
  } else if (strncasecmp (cur, "CLEAR", 5) == 0) {
    cur += 5;
    out->kind = ST_CLEAR;
    return 1;
  } else if (strncasecmp (cur, "PRINT", 5) == 0) {
    cur += 5;
    out->kind = ST_PRINT;
    out->u.print.items = NULL;
    out->u.print.n = 0;
    out->u.print.no_nl = 0;
    return 1;
  } else if (strncasecmp (cur, "INPUT", 5) == 0) {
    cur += 5;
    out->kind = ST_INPUT;
    out->u.input.var = parse_id ();
    out->u.input.is_str = out->u.input.var[strlen (out->u.input.var) - 1] == '$';
    return 1;
  } else if (strncasecmp (cur, "GET", 3) == 0) {
    cur += 3;
    out->kind = ST_GET;
    out->u.get.var = parse_id ();
    return 1;

  } else if (strncasecmp (cur, "DATA", 4) == 0) {
    cur += 4;
    out->kind = ST_DATA;
    while (1) {
      skip_ws ();
      BasicData d;
      d.is_str = 0;
      d.num = 0;
      d.str = NULL;
      if (*cur == '"') {
        d.is_str = 1;
        d.str = parse_string ();
      } else {
        d.num = parse_number ();
      }
      data_vec_push (&data_vals, d);
      skip_ws ();
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
      Node *v = parse_factor ();
      if (v == NULL) return 0;
      if (out->u.read.n == cap) {
        cap = cap ? cap * 2 : 4;
        out->u.read.vars = realloc (out->u.read.vars, cap * sizeof (Node *));
      }
      out->u.read.vars[out->u.read.n++] = v;
      skip_ws ();
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
    skip_ws ();
    out->kind = ST_SCREEN;
    out->u.expr = parse_expr ();
    return 1;
  } else if (strncasecmp (cur, "CLS", 3) == 0) {
    cur += 3;
    out->kind = ST_CLS;
    return 1;
  } else if (strncasecmp (cur, "COLOR", 5) == 0) {
    cur += 5;
    skip_ws ();
    out->kind = ST_COLOR;
    out->u.expr = parse_expr ();
    return 1;
  } else if (strncasecmp (cur, "KEY", 3) == 0) {
    cur += 3;
    skip_ws ();
    if (strncasecmp (cur, "OFF", 3) != 0) return 0;
    cur += 3;
    out->kind = ST_KEYOFF;
    return 1;
  } else if (strncasecmp (cur, "LOCATE", 6) == 0) {
    cur += 6;
    skip_ws ();
    out->kind = ST_LOCATE;
    out->u.locate.row = parse_expr ();
    skip_ws ();
    if (*cur != ',') return 0;
    cur++;
    out->u.locate.col = parse_expr ();
    return 1;
  } else if (strncasecmp (cur, "HTAB", 4) == 0 || strncasecmp (cur, "TAB", 3) == 0) {
    cur += strncasecmp (cur, "HTAB", 4) == 0 ? 4 : 3;
    skip_ws ();
    out->kind = ST_HTAB;
    out->u.expr = parse_expr ();
    return 1;
  } else if (strncasecmp (cur, "POKE", 4) == 0) {
    cur += 4;
    skip_ws ();
    out->kind = ST_POKE;
    out->u.poke.addr = parse_expr ();
    skip_ws ();
    if (*cur != ',') return 0;
    cur++;
    out->u.poke.value = parse_expr ();
    return 1;
  } else if (strncasecmp (cur, "HOME", 4) == 0) {
    cur += 4;
    out->kind = ST_HOME;
    return 1;
  } else if (strncasecmp (cur, "VTAB", 4) == 0) {
    cur += 4;
    skip_ws ();
    out->kind = ST_VTAB;
    out->u.expr = parse_expr ();
    return 1;
  } else if (strncasecmp (cur, "LET", 3) == 0) {
    cur += 3;
    skip_ws ();
    Node *v = parse_factor ();
    skip_ws ();
    if (*cur == '=') cur++;
    Node *e = parse_expr ();
    out->kind = ST_LET;
    out->u.let.var = v;
    out->u.let.expr = e;
    out->u.let.is_str = v->is_str;
    return 1;
  } else if (strncasecmp (cur, "GOTO", 4) == 0) {
    cur += 4;
    out->kind = ST_GOTO;
    out->u.target = parse_int ();
    return 1;
  } else if (strncasecmp (cur, "GOSUB", 5) == 0) {
    cur += 5;
    out->kind = ST_GOSUB;
    out->u.target = parse_int ();
    return 1;
  } else if (strncasecmp (cur, "IF", 2) == 0) {
    cur += 2;
    Stmt s;
    s.kind = ST_IF;
    s.u.iff.cond = parse_expr ();
    if (strncasecmp (cur, "THEN", 4) != 0) return 0;
    cur += 4;
    skip_ws ();
    s.u.iff.stmt = malloc (sizeof (Stmt));
    if (isdigit ((unsigned char) *cur)) {
      s.u.iff.stmt->kind = ST_GOTO;
      s.u.iff.stmt->u.target = parse_int ();
    } else {
      if (!parse_stmt (s.u.iff.stmt)) return 0;
    }
    *out = s;
    return 1;
  } else if (strncasecmp (cur, "FOR", 3) == 0) {
    cur += 3;
    out->kind = ST_FOR;
    out->u.forto.var = parse_id ();
    skip_ws ();
    if (*cur == '=') cur++;
    out->u.forto.start = parse_expr ();
    if (strncasecmp (cur, "TO", 2) != 0) return 0;
    cur += 2;
    out->u.forto.end = parse_expr ();
    if (strncasecmp (cur, "STEP", 4) == 0) {
      cur += 4;
      out->u.forto.step = parse_expr ();
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
    skip_ws ();
    if (isalpha ((unsigned char) *cur)) out->u.next.var = parse_id ();
    return 1;
  } else if (strncasecmp (cur, "WHILE", 5) == 0) {
    cur += 5;
    out->kind = ST_WHILE;
    out->u.expr = parse_expr ();
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
    Node *e = parse_expr ();
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
      int t = parse_int ();
      if (*n_targets == cap) {
        cap = cap ? cap * 2 : 4;
        *targets = realloc (*targets, cap * sizeof (int));
      }
      (*targets)[(*n_targets)++] = t;
      skip_ws ();
      if (*cur != ',') break;
      cur++;
    }
    return 1;
  } else if (strncasecmp (cur, "END", 3) == 0) {
    out->kind = ST_END;
    return 1;
  } else if (isalpha ((unsigned char) *cur)) {
    Node *v = parse_factor ();
    skip_ws ();
    if (*cur == '=') {
      cur++;
      Node *e = parse_expr ();
      out->kind = ST_LET;
      out->u.let.var = v;
      out->u.let.expr = e;
      out->u.let.is_str = v->is_str;
      return 1;
    }
  }
  return 0;
}

/* Parse a single line into multiple statements */
static int parse_line (char *line, Line *out) {
  cur = line;
  int line_no = parse_int ();
  out->line = line_no;
  out->stmts = (StmtVec) {0};
  while (1) {
    Stmt s;
    if (!parse_stmt (&s)) return 0;
    if (s.kind == ST_PRINT) {
      size_t cap = 0;
      s.u.print.items = NULL;
      s.u.print.n = 0;
      s.u.print.no_nl = 0;
      while (1) {
        skip_ws ();
        if (*cur == ':' || *cur == '\0') break;
        Node *e = parse_expr ();
        if (s.u.print.n == cap) {
          cap = cap ? cap * 2 : 4;
          s.u.print.items = realloc (s.u.print.items, cap * sizeof (Node *));
        }
        s.u.print.items[s.u.print.n++] = e;
        skip_ws ();
        if (*cur == ';' || *cur == ',') {
          cur++;
          skip_ws ();
          if (*cur == ':' || *cur == '\0') {
            s.u.print.no_nl = 1;
            break;
          }
          continue;
        }
        break;
      }
      skip_ws ();
    }
    vec_push (&out->stmts, s);
    if (s.kind == ST_REM) break;
    skip_ws ();
    if (*cur == ':') {
      cur++;
      continue;
    }
    break;
  }
  return 1;
}

/* Variable mapping */
typedef struct {
  char *name;
  MIR_reg_t reg;
  int is_str;
  int is_array;
  size_t size;
} Var;
typedef struct {
  Var *data;
  size_t len, cap;
} VarVec;
static MIR_reg_t get_var (VarVec *vars, MIR_context_t ctx, MIR_item_t func, const char *name) {
  int is_str = name[strlen (name) - 1] == '$';
  for (size_t i = 0; i < vars->len; i++)
    if (strcmp (vars->data[i].name, name) == 0) return vars->data[i].reg;
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
  return vars->data[vars->len++].reg;
}

static MIR_reg_t get_array (VarVec *vars, MIR_context_t ctx, MIR_item_t func, const char *name,
                            size_t size, int is_str) {
  for (size_t i = 0; i < vars->len; i++)
    if (strcmp (vars->data[i].name, name) == 0) {
      vars->data[i].is_array = 1;
      if (size != 0) vars->data[i].size = size;
      return vars->data[i].reg;
    }
  if (vars->len == vars->cap) {
    vars->cap = vars->cap ? 2 * vars->cap : 16;
    vars->data = realloc (vars->data, vars->cap * sizeof (Var));
  }
  vars->data[vars->len].name = strdup (name);
  vars->data[vars->len].is_str = is_str;
  vars->data[vars->len].is_array = 1;
  vars->data[vars->len].size = size;
  vars->data[vars->len].reg = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, name);
  return vars->data[vars->len++].reg;
}

/* Expression code generation */
static int tmp_id = 0;
static MIR_reg_t gen_expr (MIR_context_t ctx, MIR_item_t func, VarVec *vars, Node *n) {
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
        MIR_reg_t base = get_array (vars, ctx, func, n->var, 0, 1);
        MIR_reg_t idxd = gen_expr (ctx, func, vars, n->index);
        char buf[32];
        sprintf (buf, "$t%d", tmp_id++);
        MIR_reg_t idx = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_D2I, MIR_new_reg_op (ctx, idx),
                                       MIR_new_reg_op (ctx, idxd)));
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
      }
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
      MIR_reg_t base = get_array (vars, ctx, func, n->var, 0, 0);
      MIR_reg_t idxd = gen_expr (ctx, func, vars, n->index);
      char buf[32];
      sprintf (buf, "$t%d", tmp_id++);
      MIR_reg_t idx = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
      MIR_append_insn (ctx, func,
                       MIR_new_insn (ctx, MIR_D2I, MIR_new_reg_op (ctx, idx),
                                     MIR_new_reg_op (ctx, idxd)));
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
      MIR_reg_t val = MIR_new_func_reg (ctx, func->u.func, MIR_T_D, buf);
      MIR_append_insn (ctx, func,
                       MIR_new_insn (ctx, MIR_DMOV, MIR_new_reg_op (ctx, val),
                                     MIR_new_mem_op (ctx, MIR_T_D, 0, addr, 0, 1)));
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
    } else if (strcasecmp (n->var, "PEEK") == 0) {
      MIR_reg_t arg = gen_expr (ctx, func, vars, n->left);
      MIR_append_insn (ctx, func,
                       MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, peek_proto),
                                          MIR_new_ref_op (ctx, peek_import),
                                          MIR_new_reg_op (ctx, res), MIR_new_reg_op (ctx, arg)));
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

static void gen_program (LineVec *prog, int jit, int asm_p, int obj_p, int bin_p,
                         const char *out_name, const char *src_name) {
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
  rnd_proto = MIR_new_proto (ctx, "basic_rnd_p", 1, &d, 1, MIR_T_D, "n");
  rnd_import = MIR_new_import (ctx, "basic_rnd");
  chr_proto = MIR_new_proto (ctx, "basic_chr_p", 1, &p, 1, MIR_T_D, "n");
  chr_import = MIR_new_import (ctx, "basic_chr");
  string_proto = MIR_new_proto (ctx, "basic_string_p", 1, &p, 2, MIR_T_D, "n", MIR_T_P, "s");
  string_import = MIR_new_import (ctx, "basic_string");
  int_proto = MIR_new_proto (ctx, "basic_int_p", 1, &d, 1, MIR_T_D, "x");
  int_import = MIR_new_import (ctx, "basic_int");
  timer_proto = MIR_new_proto (ctx, "basic_timer_p", 1, &d, 0);
  timer_import = MIR_new_import (ctx, "basic_timer");
  input_chr_proto = MIR_new_proto (ctx, "basic_input_chr_p", 1, &p, 1, MIR_T_D, "n");
  input_chr_import = MIR_new_import (ctx, "basic_input_chr");
  peek_proto = MIR_new_proto (ctx, "basic_peek_p", 1, &d, 1, MIR_T_D, "addr");
  peek_import = MIR_new_import (ctx, "basic_peek");
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
  MIR_type_t res_t = MIR_T_I64;
  MIR_item_t func = MIR_new_func (ctx, "main", 1, &res_t, 0);
  VarVec vars = {0};

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

  typedef enum { LOOP_FOR, LOOP_WHILE } LoopType;
  typedef struct {
    LoopType type;
    MIR_reg_t var, end, step;
    MIR_label_t start_label, end_label;
  } LoopInfo;

  LoopInfo *loop_stack = NULL;
  size_t loop_len = 0, loop_cap = 0;

  /* create labels for lines */
  size_t n = prog->len;
  MIR_label_t *labels = malloc (sizeof (MIR_label_t) * n);
  for (size_t i = 0; i < n; i++) labels[i] = MIR_new_label (ctx);

  for (size_t i = 0; i < n; i++) {
    Line *ln = &prog->data[i];
    MIR_append_insn (ctx, func, labels[i]);
    for (size_t j = 0; j < ln->stmts.len; j++) {
      Stmt *s = &ln->stmts.data[j];
      switch (s->kind) {
      case ST_PRINT: {
        for (size_t k = 0; k < s->u.print.n; k++) {
          Node *e = s->u.print.items[k];
          MIR_reg_t r = gen_expr (ctx, func, &vars, e);
          MIR_item_t proto = e->is_str ? prints_proto : print_proto;
          MIR_item_t import = e->is_str ? prints_import : print_import;
          MIR_append_insn (ctx, func,
                           MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, proto),
                                              MIR_new_ref_op (ctx, import),
                                              MIR_new_reg_op (ctx, r)));
        }
        if (!s->u.print.no_nl)
          MIR_append_insn (ctx, func,
                           MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, prints_proto),
                                              MIR_new_ref_op (ctx, prints_import),
                                              MIR_new_str_op (ctx, (MIR_str_t) {2, "\n"})));
        break;
      }
      case ST_LET: {
        MIR_reg_t r = gen_expr (ctx, func, &vars, s->u.let.expr);
        if (s->u.let.var->index != NULL) {
          MIR_reg_t base = get_array (&vars, ctx, func, s->u.let.var->var, 0, s->u.let.is_str);
          MIR_reg_t idxd = gen_expr (ctx, func, &vars, s->u.let.var->index);
          char buf[32];
          sprintf (buf, "$t%d", tmp_id++);
          MIR_reg_t idx = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
          MIR_append_insn (ctx, func,
                           MIR_new_insn (ctx, MIR_D2I, MIR_new_reg_op (ctx, idx),
                                         MIR_new_reg_op (ctx, idxd)));
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
          MIR_insn_code_t mov = s->u.let.is_str ? MIR_MOV : MIR_DMOV;
          MIR_append_insn (ctx, func,
                           MIR_new_insn (ctx, mov,
                                         MIR_new_mem_op (ctx, s->u.let.is_str ? MIR_T_P : MIR_T_D,
                                                         0, addr, 0, 1),
                                         MIR_new_reg_op (ctx, r)));
        } else {
          MIR_reg_t v = get_var (&vars, ctx, func, s->u.let.var->var);
          MIR_insn_code_t mov = s->u.let.is_str ? MIR_MOV : MIR_DMOV;
          MIR_append_insn (ctx, func,
                           MIR_new_insn (ctx, mov, MIR_new_reg_op (ctx, v),
                                         MIR_new_reg_op (ctx, r)));
        }
        break;
      }
      case ST_GOTO: {
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_JMP,
                                       MIR_new_label_op (ctx,
                                                         find_label (prog, labels, s->u.target))));
        break;
      }
      case ST_GOSUB: {
        MIR_label_t ret = MIR_new_label (ctx);
        char buf[32];
        sprintf (buf, "$t%d", tmp_id++);
        MIR_reg_t tmp = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_MOV, MIR_new_reg_op (ctx, tmp),
                                       MIR_new_label_op (ctx, ret)));
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_MOV,
                                       MIR_new_mem_op (ctx, MIR_T_P, 0, ret_stack, ret_sp, 1),
                                       MIR_new_reg_op (ctx, tmp)));
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_ADD, MIR_new_reg_op (ctx, ret_sp),
                                       MIR_new_reg_op (ctx, ret_sp), MIR_new_int_op (ctx, 8)));
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_JMP,
                                       MIR_new_label_op (ctx,
                                                         find_label (prog, labels, s->u.target))));
        MIR_append_insn (ctx, func, ret);
        break;
      }
      case ST_IF: {
        MIR_label_t skip = MIR_new_label (ctx);
        MIR_reg_t r = gen_expr (ctx, func, &vars, s->u.iff.cond);
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_DBEQ, MIR_new_label_op (ctx, skip),
                                       MIR_new_reg_op (ctx, r), MIR_new_double_op (ctx, 0.0)));
        Stmt *bs = s->u.iff.stmt;
        if (bs->kind == ST_GOTO) {
          MIR_append_insn (ctx, func,
                           MIR_new_insn (ctx, MIR_JMP,
                                         MIR_new_label_op (ctx, find_label (prog, labels,
                                                                            bs->u.target))));
        } else if (bs->kind == ST_GOSUB) {
          MIR_label_t ret = MIR_new_label (ctx);
          char buf[32];
          sprintf (buf, "$t%d", tmp_id++);
          MIR_reg_t tmp = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
          MIR_append_insn (ctx, func,
                           MIR_new_insn (ctx, MIR_MOV, MIR_new_reg_op (ctx, tmp),
                                         MIR_new_label_op (ctx, ret)));
          MIR_append_insn (ctx, func,
                           MIR_new_insn (ctx, MIR_MOV,
                                         MIR_new_mem_op (ctx, MIR_T_P, 0, ret_stack, ret_sp, 1),
                                         MIR_new_reg_op (ctx, tmp)));
          MIR_append_insn (ctx, func,
                           MIR_new_insn (ctx, MIR_ADD, MIR_new_reg_op (ctx, ret_sp),
                                         MIR_new_reg_op (ctx, ret_sp), MIR_new_int_op (ctx, 8)));
          MIR_append_insn (ctx, func,
                           MIR_new_insn (ctx, MIR_JMP,
                                         MIR_new_label_op (ctx, find_label (prog, labels,
                                                                            bs->u.target))));
          MIR_append_insn (ctx, func, ret);
        }
        MIR_append_insn (ctx, func, skip);
        break;
      }
      case ST_INPUT: {
        MIR_reg_t v = get_var (&vars, ctx, func, s->u.input.var);
        if (s->u.input.is_str) {
          MIR_append_insn (ctx, func,
                           MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, input_str_proto),
                                              MIR_new_ref_op (ctx, input_str_import),
                                              MIR_new_reg_op (ctx, v)));
        } else {
          MIR_append_insn (ctx, func,
                           MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, input_proto),
                                              MIR_new_ref_op (ctx, input_import),
                                              MIR_new_reg_op (ctx, v)));
        }
        break;
      }
      case ST_GET: {
        MIR_reg_t v = get_var (&vars, ctx, func, s->u.get.var);
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, get_proto),
                                            MIR_new_ref_op (ctx, get_import),
                                            MIR_new_reg_op (ctx, v)));
        break;
      }
      case ST_DATA: {
        break;
      }
      case ST_READ: {
        for (size_t ri = 0; ri < s->u.read.n; ri++) {
          Node *v = s->u.read.vars[ri];
          MIR_reg_t r;
          if (v->is_str) {
            char buf[32];
            sprintf (buf, "$t%d", tmp_id++);
            r = MIR_new_func_reg (ctx, func->u.func, MIR_T_P, buf);
            MIR_append_insn (ctx, func,
                             MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, read_str_proto),
                                                MIR_new_ref_op (ctx, read_str_import),
                                                MIR_new_reg_op (ctx, r)));
          } else {
            char buf[32];
            sprintf (buf, "$t%d", tmp_id++);
            r = MIR_new_func_reg (ctx, func->u.func, MIR_T_D, buf);
            MIR_append_insn (ctx, func,
                             MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, read_proto),
                                                MIR_new_ref_op (ctx, read_import),
                                                MIR_new_reg_op (ctx, r)));
          }
          if (v->index != NULL) {
            MIR_reg_t base = get_array (&vars, ctx, func, v->var, 0, v->is_str);
            MIR_reg_t idxd = gen_expr (ctx, func, &vars, v->index);
            char buf[32];
            sprintf (buf, "$t%d", tmp_id++);
            MIR_reg_t idx = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
            MIR_append_insn (ctx, func,
                             MIR_new_insn (ctx, MIR_D2I, MIR_new_reg_op (ctx, idx),
                                           MIR_new_reg_op (ctx, idxd)));
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
            MIR_insn_code_t mov = v->is_str ? MIR_MOV : MIR_DMOV;
            MIR_append_insn (ctx, func,
                             MIR_new_insn (ctx, mov,
                                           MIR_new_mem_op (ctx, v->is_str ? MIR_T_P : MIR_T_D, 0,
                                                           addr, 0, 1),
                                           MIR_new_reg_op (ctx, r)));
          } else {
            MIR_reg_t dest = get_var (&vars, ctx, func, v->var);
            MIR_insn_code_t mov = v->is_str ? MIR_MOV : MIR_DMOV;
            MIR_append_insn (ctx, func,
                             MIR_new_insn (ctx, mov, MIR_new_reg_op (ctx, dest),
                                           MIR_new_reg_op (ctx, r)));
          }
        }
        break;
      }
      case ST_RESTORE: {
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 2, MIR_new_ref_op (ctx, restore_proto),
                                            MIR_new_ref_op (ctx, restore_import)));
        break;
      }
      case ST_SCREEN: {
        MIR_reg_t r = gen_expr (ctx, func, &vars, s->u.expr);
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, screen_proto),
                                            MIR_new_ref_op (ctx, screen_import),
                                            MIR_new_reg_op (ctx, r)));
        break;
      }
      case ST_CLS: {
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 2, MIR_new_ref_op (ctx, cls_proto),
                                            MIR_new_ref_op (ctx, cls_import)));
        break;
      }
      case ST_COLOR: {
        MIR_reg_t r = gen_expr (ctx, func, &vars, s->u.expr);
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, color_proto),
                                            MIR_new_ref_op (ctx, color_import),
                                            MIR_new_reg_op (ctx, r)));
        break;
      }
      case ST_KEYOFF: {
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 2, MIR_new_ref_op (ctx, keyoff_proto),
                                            MIR_new_ref_op (ctx, keyoff_import)));
        break;
      }
      case ST_LOCATE: {
        MIR_reg_t r = gen_expr (ctx, func, &vars, s->u.locate.row);
        MIR_reg_t c = gen_expr (ctx, func, &vars, s->u.locate.col);
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, locate_proto),
                                            MIR_new_ref_op (ctx, locate_import),
                                            MIR_new_reg_op (ctx, r), MIR_new_reg_op (ctx, c)));
        break;
      }
      case ST_HTAB: {
        MIR_reg_t r = gen_expr (ctx, func, &vars, s->u.expr);
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, htab_proto),
                                            MIR_new_ref_op (ctx, htab_import),
                                            MIR_new_reg_op (ctx, r)));
        break;
      }
      case ST_POKE: {
        MIR_reg_t a = gen_expr (ctx, func, &vars, s->u.poke.addr);
        MIR_reg_t v = gen_expr (ctx, func, &vars, s->u.poke.value);
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 4, MIR_new_ref_op (ctx, poke_proto),
                                            MIR_new_ref_op (ctx, poke_import),
                                            MIR_new_reg_op (ctx, a), MIR_new_reg_op (ctx, v)));
        break;
      }
      case ST_HOME: {
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 2, MIR_new_ref_op (ctx, home_proto),
                                            MIR_new_ref_op (ctx, home_import)));
        break;
      }
      case ST_VTAB: {
        MIR_reg_t r = gen_expr (ctx, func, &vars, s->u.expr);
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, vtab_proto),
                                            MIR_new_ref_op (ctx, vtab_import),
                                            MIR_new_reg_op (ctx, r)));
        break;
      }
      case ST_CLEAR: {
        for (size_t vi = 0; vi < vars.len; vi++) {
          Var *v = &vars.data[vi];
          if (v->is_array) {
            size_t total = v->size * 8;
            MIR_append_insn (ctx, func,
                             MIR_new_call_insn (ctx, 6, MIR_new_ref_op (ctx, memset_proto),
                                                MIR_new_ref_op (ctx, memset_import),
                                                MIR_new_reg_op (ctx, v->reg),
                                                MIR_new_reg_op (ctx, v->reg),
                                                MIR_new_int_op (ctx, 0),
                                                MIR_new_int_op (ctx, total)));
          } else {
            MIR_insn_code_t mov = v->is_str ? MIR_MOV : MIR_DMOV;
            MIR_append_insn (ctx, func,
                             MIR_new_insn (ctx, mov, MIR_new_reg_op (ctx, v->reg),
                                           v->is_str ? MIR_new_int_op (ctx, 0)
                                                     : MIR_new_double_op (ctx, 0.0)));
          }
        }
        break;
      }
      case ST_FOR: {
        MIR_reg_t var = get_var (&vars, ctx, func, s->u.forto.var);
        MIR_reg_t start = gen_expr (ctx, func, &vars, s->u.forto.start);
        MIR_reg_t end = gen_expr (ctx, func, &vars, s->u.forto.end);
        MIR_reg_t step = gen_expr (ctx, func, &vars, s->u.forto.step);
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_DMOV, MIR_new_reg_op (ctx, var),
                                       MIR_new_reg_op (ctx, start)));
        MIR_label_t pos = MIR_new_label (ctx);
        MIR_label_t body = MIR_new_label (ctx);
        MIR_label_t loop_end = MIR_new_label (ctx);
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_DBGE, MIR_new_label_op (ctx, pos),
                                       MIR_new_reg_op (ctx, step), MIR_new_double_op (ctx, 0.0)));
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_DBLT, MIR_new_label_op (ctx, loop_end),
                                       MIR_new_reg_op (ctx, var), MIR_new_reg_op (ctx, end)));
        MIR_append_insn (ctx, func, MIR_new_insn (ctx, MIR_JMP, MIR_new_label_op (ctx, body)));
        MIR_append_insn (ctx, func, pos);
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_DBGT, MIR_new_label_op (ctx, loop_end),
                                       MIR_new_reg_op (ctx, var), MIR_new_reg_op (ctx, end)));
        MIR_append_insn (ctx, func, body);
        if (loop_len == loop_cap) {
          loop_cap = loop_cap ? 2 * loop_cap : 4;
          loop_stack = realloc (loop_stack, loop_cap * sizeof (LoopInfo));
        }
        loop_stack[loop_len++] = (LoopInfo) {LOOP_FOR, var, end, step, body, loop_end};
        break;
      }
      case ST_NEXT: {
        if (loop_len == 0) break;
        LoopInfo info = loop_stack[loop_len - 1];
        if (info.type != LOOP_FOR) break;
        loop_len--;
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_DADD, MIR_new_reg_op (ctx, info.var),
                                       MIR_new_reg_op (ctx, info.var),
                                       MIR_new_reg_op (ctx, info.step)));
        MIR_label_t pos = MIR_new_label (ctx);
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_DBGE, MIR_new_label_op (ctx, pos),
                                       MIR_new_reg_op (ctx, info.step),
                                       MIR_new_double_op (ctx, 0.0)));
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_DBLT, MIR_new_label_op (ctx, info.end_label),
                                       MIR_new_reg_op (ctx, info.var),
                                       MIR_new_reg_op (ctx, info.end)));
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_JMP, MIR_new_label_op (ctx, info.start_label)));
        MIR_append_insn (ctx, func, pos);
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_DBGT, MIR_new_label_op (ctx, info.end_label),
                                       MIR_new_reg_op (ctx, info.var),
                                       MIR_new_reg_op (ctx, info.end)));
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_JMP, MIR_new_label_op (ctx, info.start_label)));
        MIR_append_insn (ctx, func, info.end_label);
        break;
      }
      case ST_WHILE: {
        MIR_label_t start = MIR_new_label (ctx);
        MIR_label_t end = MIR_new_label (ctx);
        MIR_append_insn (ctx, func, start);
        MIR_reg_t cond = gen_expr (ctx, func, &vars, s->u.expr);
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_DBEQ, MIR_new_label_op (ctx, end),
                                       MIR_new_reg_op (ctx, cond), MIR_new_double_op (ctx, 0.0)));
        if (loop_len == loop_cap) {
          loop_cap = loop_cap ? 2 * loop_cap : 4;
          loop_stack = realloc (loop_stack, loop_cap * sizeof (LoopInfo));
        }
        loop_stack[loop_len++] = (LoopInfo) {LOOP_WHILE, 0, 0, 0, start, end};
        break;
      }
      case ST_WEND: {
        if (loop_len == 0) break;
        LoopInfo info = loop_stack[loop_len - 1];
        if (info.type != LOOP_WHILE) break;
        loop_len--;
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_JMP, MIR_new_label_op (ctx, info.start_label)));
        MIR_append_insn (ctx, func, info.end_label);
        break;
      }
      case ST_RETURN: {
        if (ret_addr == 0) ret_addr = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, "ret_addr");
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_SUB, MIR_new_reg_op (ctx, ret_sp),
                                       MIR_new_reg_op (ctx, ret_sp), MIR_new_int_op (ctx, 8)));
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_MOV, MIR_new_reg_op (ctx, ret_addr),
                                       MIR_new_mem_op (ctx, MIR_T_P, 0, ret_stack, ret_sp, 1)));
        MIR_append_insn (ctx, func, MIR_new_insn (ctx, MIR_JMPI, MIR_new_reg_op (ctx, ret_addr)));
        break;
      }
      case ST_ON_GOTO: {
        MIR_reg_t expr = gen_expr (ctx, func, &vars, s->u.on_goto.expr);
        char buf[32];
        sprintf (buf, "$t%d", tmp_id++);
        MIR_reg_t idx = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_D2I, MIR_new_reg_op (ctx, idx),
                                       MIR_new_reg_op (ctx, expr)));
        MIR_label_t after = MIR_new_label (ctx);
        for (size_t k = 0; k < s->u.on_goto.n_targets; k++) {
          MIR_label_t next = k + 1 < s->u.on_goto.n_targets ? MIR_new_label (ctx) : after;
          MIR_append_insn (ctx, func,
                           MIR_new_insn (ctx, MIR_BNE, MIR_new_label_op (ctx, next),
                                         MIR_new_reg_op (ctx, idx),
                                         MIR_new_int_op (ctx, (int) k + 1)));
          MIR_append_insn (ctx, func,
                           MIR_new_insn (ctx, MIR_JMP,
                                         MIR_new_label_op (ctx,
                                                           find_label (prog, labels,
                                                                       s->u.on_goto.targets[k]))));
          if (k + 1 < s->u.on_goto.n_targets) MIR_append_insn (ctx, func, next);
        }
        MIR_append_insn (ctx, func, after);
        break;
      }
      case ST_ON_GOSUB: {
        MIR_reg_t expr = gen_expr (ctx, func, &vars, s->u.on_gosub.expr);
        char buf[32];
        sprintf (buf, "$t%d", tmp_id++);
        MIR_reg_t idx = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_D2I, MIR_new_reg_op (ctx, idx),
                                       MIR_new_reg_op (ctx, expr)));
        MIR_label_t after = MIR_new_label (ctx);
        for (size_t k = 0; k < s->u.on_gosub.n_targets; k++) {
          MIR_label_t next = k + 1 < s->u.on_gosub.n_targets ? MIR_new_label (ctx) : after;
          MIR_append_insn (ctx, func,
                           MIR_new_insn (ctx, MIR_BNE, MIR_new_label_op (ctx, next),
                                         MIR_new_reg_op (ctx, idx),
                                         MIR_new_int_op (ctx, (int) k + 1)));
          MIR_append_insn (ctx, func,
                           MIR_new_insn (ctx, MIR_MOV,
                                         MIR_new_mem_op (ctx, MIR_T_P, 0, ret_stack, ret_sp, 1),
                                         MIR_new_label_op (ctx, after)));
          MIR_append_insn (ctx, func,
                           MIR_new_insn (ctx, MIR_ADD, MIR_new_reg_op (ctx, ret_sp),
                                         MIR_new_reg_op (ctx, ret_sp), MIR_new_int_op (ctx, 8)));
          MIR_append_insn (ctx, func,
                           MIR_new_insn (ctx, MIR_JMP,
                                         MIR_new_label_op (ctx,
                                                           find_label (prog, labels,
                                                                       s->u.on_gosub.targets[k]))));
          if (k + 1 < s->u.on_gosub.n_targets) MIR_append_insn (ctx, func, next);
        }
        MIR_append_insn (ctx, func, after);
        break;
      }
      case ST_END: {
        MIR_append_insn (ctx, func, MIR_new_ret_insn (ctx, 1, MIR_new_int_op (ctx, 0)));
        break;
      }
      case ST_DIM: {
        for (size_t k = 0; k < s->u.dim.n; k++) {
          MIR_reg_t base = get_array (&vars, ctx, func, s->u.dim.names[k], s->u.dim.sizes[k],
                                      s->u.dim.is_str[k]);
          MIR_append_insn (ctx, func,
                           MIR_new_call_insn (ctx, 5, MIR_new_ref_op (ctx, calloc_proto),
                                              MIR_new_ref_op (ctx, calloc_import),
                                              MIR_new_reg_op (ctx, base),
                                              MIR_new_int_op (ctx, s->u.dim.sizes[k]),
                                              MIR_new_int_op (ctx, 8)));
        }
        break;
      }
      case ST_REM:
        /* comment */
        break;
      }
    }
  }
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
      size_t size
        = strlen (cc) + strlen (src_dir) * 5 + strlen (ctab_name) + strlen (exe_name) + 200;
      char *cmd = malloc (size);
      snprintf (cmd, size,
                "%s -I\"%s\" -DCTAB_INCLUDE_STRING=\\\"%s\\\" \"%s/mir-bin-driver.c\" "
                "\"%s/examples/basic/basic_runtime.c\" \"%s/mir.c\" \"%s/mir-gen.c\" -rdynamic -lm "
                "-ldl -o "
                "\"%s\"",
                cc, src_dir, ctab_name, src_dir, src_dir, src_dir, src_dir, exe_name);
      if (system (cmd) != 0) perror (cmd);
      free (cmd);
    } else {
      perror (ctab_name);
    }
    free (ctab_name);
    free (exe_name);
    MIR_finish (ctx);
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
}

int main (int argc, char **argv) {
  int jit = 0, asm_p = 0, obj_p = 0, bin_p = 0;
  const char *fname = NULL, *out_name = NULL;
  for (int i = 1; i < argc; i++) {
    if (strcmp (argv[i], "-j") == 0) {
      jit = 1;
    } else if (strcmp (argv[i], "-S") == 0) {
      asm_p = obj_p = 1;
    } else if (strcmp (argv[i], "-c") == 0) {
      asm_p = obj_p = 1;
    } else if (strcmp (argv[i], "-b") == 0) {
      bin_p = 1;
    } else if (strcmp (argv[i], "-o") == 0 && i + 1 < argc) {
      out_name = argv[++i];
    } else {
      fname = argv[i];
    }
  }
  if (!fname) {
    fprintf (stderr, "usage: %s [-j] [-S|-c] [-b] [-o output] file.bas\n", argv[0]);
    return 1;
  }
  FILE *f = fopen (fname, "r");
  if (!f) {
    perror (fname);
    return 1;
  }
  LineVec prog = {0};
  char line[256];
  while (fgets (line, sizeof (line), f)) {
    Line l;
    if (parse_line (line, &l))
      line_vec_push (&prog, l);
    else
      fprintf (stderr, "parse error: %s\n", line);
  }
  fclose (f);
  gen_program (&prog, jit, asm_p, obj_p, bin_p, out_name, fname);
  return 0;
}

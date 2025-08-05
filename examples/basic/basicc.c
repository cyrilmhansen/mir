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

static void *resolve (const char *name) {
  if (!strcmp (name, "basic_print")) return basic_print;
  if (!strcmp (name, "basic_print_str")) return basic_print_str;
  if (!strcmp (name, "basic_input")) return basic_input;
  if (!strcmp (name, "basic_input_str")) return basic_input_str;
  if (!strcmp (name, "basic_get")) return basic_get;
  if (!strcmp (name, "basic_strcmp")) return basic_strcmp;
  return NULL;
}

/* AST for expressions */
typedef enum { N_NUM, N_VAR, N_BIN, N_NEG, N_STR } NodeKind;
typedef struct Node Node;
struct Node {
  NodeKind kind;
  int is_str;
  double num;
  char *var;
  char *str;
  char op;
  Node *left, *right;
};

static Node *new_node (NodeKind k) {
  Node *n = malloc (sizeof (Node));
  n->kind = k;
  n->is_str = 0;
  n->num = 0;
  n->var = NULL;
  n->str = NULL;
  n->op = 0;
  n->left = n->right = NULL;
  return n;
}

/* Statement representation */
/* Added ST_REM for comment lines and ST_DIM for dimension declarations (ignored). */
typedef enum {
  ST_PRINT,
  ST_PRINTS,
  ST_LET,
  ST_GOTO,
  ST_IF,
  ST_INPUT,
  ST_GET,
  ST_END,
  ST_REM,
  ST_DIM,
} StmtKind;
typedef enum { REL_NONE, REL_EQ, REL_NE, REL_LT, REL_LE, REL_GT, REL_GE } Relop;
typedef struct {
  StmtKind kind;
  union {
    Node *expr; /* PRINT/PRINTS */
    struct {
      char *var;
      Node *expr;
      int is_str;
    } let;
    int target; /* GOTO */
    struct {
      Relop rel;
      Node *lhs;
      Node *rhs;
      Node *expr;
      int target;
    } iff;
    struct {
      char *var;
      int is_str;
    } input;
    struct {
      char *var;
    } get;
  } u;
} Stmt;

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
  if (*cur == '$') {
    buf[i++] = '$';
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
static Node *parse_expr (void);

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
    Node *n = new_node (N_VAR);
    n->var = parse_id ();
    n->is_str = n->var[strlen (n->var) - 1] == '$';
    return n;
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

static Node *parse_expr (void) {
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
    /* Currently we only support single dimension and ignore content */
    cur += 3;
    out->kind = ST_DIM;
    return 1;
  } else if (strncasecmp (cur, "PRINT", 5) == 0) {
    cur += 5;
    skip_ws ();
    Node *e = parse_expr ();
    if (e != NULL && e->is_str) {
      out->kind = ST_PRINTS;
    } else {
      out->kind = ST_PRINT;
    }
    out->u.expr = e;
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
  } else if (strncasecmp (cur, "LET", 3) == 0) {
    cur += 3;
    skip_ws ();
    char *v = parse_id ();
    skip_ws ();
    if (*cur == '=') cur++;
    Node *e = parse_expr ();
    out->kind = ST_LET;
    out->u.let.var = v;
    out->u.let.expr = e;
    out->u.let.is_str = v[strlen (v) - 1] == '$';
    return 1;
  } else if (strncasecmp (cur, "GOTO", 4) == 0) {
    cur += 4;
    out->kind = ST_GOTO;
    out->u.target = parse_int ();
    return 1;
  } else if (strncasecmp (cur, "IF", 2) == 0) {
    cur += 2;
    Stmt s;
    s.kind = ST_IF;
    s.u.iff.rel = REL_NONE;
    s.u.iff.expr = NULL;
    s.u.iff.lhs = NULL;
    s.u.iff.rhs = NULL;
    Node *lhs = parse_expr ();
    Relop r = parse_relop ();
    if (r != REL_NONE) {
      Node *rhs = parse_expr ();
      s.u.iff.rel = r;
      s.u.iff.lhs = lhs;
      s.u.iff.rhs = rhs;
    } else {
      s.u.iff.expr = lhs;
    }
    if (strncasecmp (cur, "THEN", 4) == 0) {
      cur += 4;
      s.u.iff.target = parse_int ();
      *out = s;
      return 1;
    }
    return 0;
  } else if (strncasecmp (cur, "END", 3) == 0) {
    out->kind = ST_END;
    return 1;
  } else if (isalpha ((unsigned char) *cur)) {
    char *v = parse_id ();
    skip_ws ();
    if (*cur == '=') {
      cur++;
      Node *e = parse_expr ();
      out->kind = ST_LET;
      out->u.let.var = v;
      out->u.let.expr = e;
      out->u.let.is_str = v[strlen (v) - 1] == '$';
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
  MIR_type_t t = is_str ? MIR_T_I64 : MIR_T_D;
  vars->data[vars->len].reg = MIR_new_func_reg (ctx, func->u.func, t, name);
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
      return get_var (vars, ctx, func, n->var);
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
    return get_var (vars, ctx, func, n->var);
  } else if (n->kind == N_NEG) {
    MIR_reg_t v = gen_expr (ctx, func, vars, n->left);
    char buf[32];
    sprintf (buf, "$t%d", tmp_id++);
    MIR_reg_t r = MIR_new_func_reg (ctx, func->u.func, MIR_T_D, buf);
    MIR_append_insn (ctx, func,
                     MIR_new_insn (ctx, MIR_DNEG, MIR_new_reg_op (ctx, r),
                                   MIR_new_reg_op (ctx, v)));
    return r;
  } else {
    MIR_reg_t l = gen_expr (ctx, func, vars, n->left);
    MIR_reg_t r = gen_expr (ctx, func, vars, n->right);
    char buf[32];
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
  MIR_item_t print_proto = MIR_new_proto (ctx, "basic_print_p", 0, NULL, 1, MIR_T_D, "x");
  MIR_item_t print_import = MIR_new_import (ctx, "basic_print");
  MIR_item_t prints_proto = MIR_new_proto (ctx, "basic_print_str_p", 0, NULL, 1, MIR_T_P, "s");
  MIR_item_t prints_import = MIR_new_import (ctx, "basic_print_str");
  MIR_type_t d = MIR_T_D;
  MIR_item_t input_proto = MIR_new_proto (ctx, "basic_input_p", 1, &d, 0);
  MIR_item_t input_import = MIR_new_import (ctx, "basic_input");
  MIR_type_t p = MIR_T_P;
  MIR_item_t input_str_proto = MIR_new_proto (ctx, "basic_input_str_p", 1, &p, 0);
  MIR_item_t input_str_import = MIR_new_import (ctx, "basic_input_str");
  MIR_item_t get_proto = MIR_new_proto (ctx, "basic_get_p", 1, &p, 0);
  MIR_item_t get_import = MIR_new_import (ctx, "basic_get");
  MIR_type_t i64 = MIR_T_I64;
  MIR_item_t strcmp_proto
    = MIR_new_proto (ctx, "basic_strcmp_p", 1, &i64, 2, MIR_T_P, "a", MIR_T_P, "b");
  MIR_item_t strcmp_import = MIR_new_import (ctx, "basic_strcmp");
  MIR_type_t res_t = MIR_T_I64;
  MIR_item_t func = MIR_new_func (ctx, "main", 1, &res_t, 0);
  VarVec vars = {0};

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
        MIR_reg_t r = gen_expr (ctx, func, &vars, s->u.expr);
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, print_proto),
                                            MIR_new_ref_op (ctx, print_import),
                                            MIR_new_reg_op (ctx, r)));
        break;
      }
      case ST_PRINTS: {
        MIR_reg_t r = gen_expr (ctx, func, &vars, s->u.expr);
        MIR_append_insn (ctx, func,
                         MIR_new_call_insn (ctx, 3, MIR_new_ref_op (ctx, prints_proto),
                                            MIR_new_ref_op (ctx, prints_import),
                                            MIR_new_reg_op (ctx, r)));
        break;
      }
      case ST_LET: {
        MIR_reg_t r = gen_expr (ctx, func, &vars, s->u.let.expr);
        MIR_reg_t v = get_var (&vars, ctx, func, s->u.let.var);
        MIR_insn_code_t mov = s->u.let.is_str ? MIR_MOV : MIR_DMOV;
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, mov, MIR_new_reg_op (ctx, v), MIR_new_reg_op (ctx, r)));
        break;
      }
      case ST_GOTO: {
        MIR_append_insn (ctx, func,
                         MIR_new_insn (ctx, MIR_JMP,
                                       MIR_new_label_op (ctx,
                                                         find_label (prog, labels, s->u.target))));
        break;
      }
      case ST_IF: {
        if (s->u.iff.rel == REL_NONE) {
          MIR_reg_t r = gen_expr (ctx, func, &vars, s->u.iff.expr);
          MIR_append_insn (ctx, func,
                           MIR_new_insn (ctx, MIR_DBNE,
                                         MIR_new_label_op (ctx, find_label (prog, labels,
                                                                            s->u.iff.target)),
                                         MIR_new_reg_op (ctx, r), MIR_new_double_op (ctx, 0.0)));
        } else if (s->u.iff.lhs->is_str) {
          MIR_reg_t l = gen_expr (ctx, func, &vars, s->u.iff.lhs);
          MIR_reg_t r = gen_expr (ctx, func, &vars, s->u.iff.rhs);
          char buf[32];
          sprintf (buf, "$t%d", tmp_id++);
          MIR_reg_t cmp = MIR_new_func_reg (ctx, func->u.func, MIR_T_I64, buf);
          MIR_append_insn (ctx, func,
                           MIR_new_call_insn (ctx, 5, MIR_new_ref_op (ctx, strcmp_proto),
                                              MIR_new_ref_op (ctx, strcmp_import),
                                              MIR_new_reg_op (ctx, cmp), MIR_new_reg_op (ctx, l),
                                              MIR_new_reg_op (ctx, r)));
          MIR_insn_code_t code = MIR_BEQ;
          switch (s->u.iff.rel) {
          case REL_EQ: code = MIR_BEQ; break;
          case REL_NE: code = MIR_BNE; break;
          case REL_LT: code = MIR_BLT; break;
          case REL_LE: code = MIR_BLE; break;
          case REL_GT: code = MIR_BGT; break;
          case REL_GE: code = MIR_BGE; break;
          default: code = MIR_BEQ; break;
          }
          MIR_append_insn (ctx, func,
                           MIR_new_insn (ctx, code,
                                         MIR_new_label_op (ctx, find_label (prog, labels,
                                                                            s->u.iff.target)),
                                         MIR_new_reg_op (ctx, cmp), MIR_new_int_op (ctx, 0)));
        } else {
          MIR_reg_t l = gen_expr (ctx, func, &vars, s->u.iff.lhs);
          MIR_reg_t r = gen_expr (ctx, func, &vars, s->u.iff.rhs);
          MIR_insn_code_t code = MIR_DBEQ;
          switch (s->u.iff.rel) {
          case REL_EQ: code = MIR_DBEQ; break;
          case REL_NE: code = MIR_DBNE; break;
          case REL_LT: code = MIR_DBLT; break;
          case REL_LE: code = MIR_DBLE; break;
          case REL_GT: code = MIR_DBGT; break;
          case REL_GE: code = MIR_DBGE; break;
          default: code = MIR_DBEQ; break;
          }
          MIR_append_insn (ctx, func,
                           MIR_new_insn (ctx, code,
                                         MIR_new_label_op (ctx, find_label (prog, labels,
                                                                            s->u.iff.target)),
                                         MIR_new_reg_op (ctx, l), MIR_new_reg_op (ctx, r)));
        }
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
      case ST_END: {
        MIR_append_insn (ctx, func, MIR_new_ret_insn (ctx, 1, MIR_new_int_op (ctx, 0)));
        break;
      }
      case ST_REM:
      case ST_DIM:
        /* no code generation needed for comments or DIM statements */
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

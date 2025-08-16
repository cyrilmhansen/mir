#include <assert.h>
#define main basicc_main
#include "../src/basicc.c"
#undef main

int main (void) {
  basic_pool_init (1024);
  arena_init (&ast_arena);
  Parser p;
  Line l;
  int ok;

  ok = parse_line (&p, "PRINT 1", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_PRINT);

  ok = parse_line (&p, "INPUT A", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_INPUT);

  ok = parse_line (&p, "GOTO 10", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_GOTO);
  assert (l.stmts.data[0].u.target == 10);

  ok = parse_line (&p, "LET A=1", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_LET);

  ok = parse_line (&p, "INC A", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_INC);

  ok = parse_line (&p, "DEC A", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_DEC);

  ok = parse_line (&p, "IF 1 THEN PRINT 1", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_IF);
  assert (l.stmts.data[0].u.iff.then_stmts.len == 1);
  assert (l.stmts.data[0].u.iff.then_stmts.data[0].kind == ST_PRINT);

  basic_pool_destroy ();
  arena_release (&ast_arena);
  return 0;
}

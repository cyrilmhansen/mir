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
  assert (l.stmts.data[0].u.print.n == 1);

  ok = parse_line (&p, "PRINT 1,2;", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_PRINT);
  assert (l.stmts.data[0].u.print.n == 2);
  assert (l.stmts.data[0].u.print.no_nl);

  ok = parse_line (&p, "PRINT#1,1;2", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_PRINT_HASH);
  assert (l.stmts.data[0].u.printhash.n == 2);

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
  assert (l.stmts.data[0].u.iff.then_stmts.data[0].u.print.n == 1);

  ok = parse_line (&p, "DIM A(10)", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_DIM);
  assert (l.stmts.data[0].u.dim.n == 1);

  ok = parse_line (&p, "CLEAR", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_CLEAR);

  ok = parse_line (&p, "RESTORE", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_RESTORE);

  ok = parse_line (&p, "CLS", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_CLS);

  ok = parse_line (&p, "KEYOFF", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_KEYOFF);

  ok = parse_line (&p, "HOME", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_HOME);

  ok = parse_line (&p, "DELAY 1", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_DELAY);

  ok = parse_line (&p, "BEEP", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_BEEP);

  ok = parse_line (&p, "CHAIN \"F\"", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_CHAIN);

  ok = parse_line (&p, "EVAL PRINT 1", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_EVAL);

  ok = parse_line (&p, "MAT A=B", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_MAT);

  ok = parse_line (&p, "TEXT", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_TEXT);

  ok = parse_line (&p, "INVERSE", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_INVERSE);

  ok = parse_line (&p, "NORMAL", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_NORMAL);

  ok = parse_line (&p, "HGR2", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_HGR2);

  ok = parse_line (&p, "RANDOMIZE", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_RANDOMIZE);

  ok = parse_line (&p, "OPEN 1, \"A\"", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_OPEN);

  ok = parse_line (&p, "CLOSE 1", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_CLOSE);

  ok = parse_line (&p, "PRINT#1,1", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_PRINT_HASH);

  ok = parse_line (&p, "INPUT#1,A$", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_INPUT_HASH);

  ok = parse_line (&p, "GET#1,A$", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_GET_HASH);

  ok = parse_line (&p, "GET A$", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_GET);

  ok = parse_line (&p, "PUT#1,1", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_PUT_HASH);

  ok = parse_line (&p, "PUT 1", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_PUT);

  ok = parse_line (&p, "SWAP A,B", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_SWAP);

  ok = parse_line (&p, "GOSUB 10", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_GOSUB);

  ok = parse_line (&p, "NEXT A", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_NEXT);

  ok = parse_line (&p, "WHILE 1", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_WHILE);

  ok = parse_line (&p, "WEND", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_WEND);

  ok = parse_line (&p, "DO", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_DO);

  ok = parse_line (&p, "LOOP", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_LOOP);

  ok = parse_line (&p, "REPEAT", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_REPEAT);

  ok = parse_line (&p, "UNTIL 1", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_UNTIL);

  ok = parse_line (&p, "RETURN", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_RETURN);

  ok = parse_line (&p, "END", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_END);

  ok = parse_line (&p, "STOP", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_STOP);

  ok = parse_line (&p, "REM", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_REM);

  ok = parse_line (&p, "OPTION BASE 0", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_REM);

  ok = parse_line (&p, "DECIMAL", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_REM);

  ok = parse_line (&p, "EXTERN F", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_EXTERN);

  ok = parse_line (&p, "DEF FNA(X)=X", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_DEF);

  ok = parse_line (&p, "DATA 1", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_DATA);

  ok = parse_line (&p, "READ A", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_READ);

  ok = parse_line (&p, "SCREEN 1", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_SCREEN);

  ok = parse_line (&p, "COLOR 1", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_COLOR);

  ok = parse_line (&p, "LOCATE 1,2", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_LOCATE);

  ok = parse_line (&p, "HTAB 1", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_HTAB);

  ok = parse_line (&p, "POKE 1,2", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_POKE);

  ok = parse_line (&p, "VTAB 1", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_VTAB);

  ok = parse_line (&p, "SOUND 1,1", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_SOUND);

  ok = parse_line (&p, "SYSTEM \"LS\",A,B$", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_SYSTEM);

  ok = parse_line (&p, "HCOLOR=1", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_HCOLOR);

  ok = parse_line (&p, "HPLOT 1,1", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_HPLOT);

  ok = parse_line (&p, "MOVE 1,2", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_MOVE);

  ok = parse_line (&p, "DRAW 1,2", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_DRAW);

  ok = parse_line (&p, "LINE 1,1 TO 2,2", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_LINE);

  ok = parse_line (&p, "CIRCLE 1,2,3", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_CIRCLE);

  ok = parse_line (&p, "RECT 1,1,2,2", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_RECT);

  ok = parse_line (&p, "MODE 1", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_MODE);

  ok = parse_line (&p, "FILL 1,1,2,2", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_FILL);

  ok = parse_line (&p, "FOR I=1 TO 10", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_FOR);

  ok = parse_line (&p, "ON 1 GOTO 10,20", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_ON_GOTO);

  ok = parse_line (&p, "RESUME 10", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_RESUME);

  ok = parse_line (&p, "CALL F", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_CALL);

  ok = parse_line (&p, "FOO", &l);
  if (!ok) return 1;
  assert (l.stmts.len == 1);
  assert (l.stmts.data[0].kind == ST_CALL);

  basic_pool_destroy ();
  arena_release (&ast_arena);
  return 0;
}

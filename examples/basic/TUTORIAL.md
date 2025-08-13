# BASICc Dialect Tutorial

This tutorial introduces the BASIC dialect implemented by the `basicc` compiler. It starts with traditional numbered BASIC, then moves to modern extensions such as user functions and optional line numbers. Work‑in‑progress graphics commands and less common syntax are also covered.

## Classical BASIC

The dialect supports line‑numbered programs in the style of early microcomputers:

```basic
10 PRINT "HELLO, WORLD"
20 INPUT "NAME? "; N$
30 IF N$ = "" THEN 20
40 PRINT "HI "; N$
50 FOR I = 1 TO 10
60   PRINT I
70 NEXT I
80 END
```

Assignments may optionally use the classic `LET` keyword, though it is not required:

```basic
LET X = 1  ' explicit LET
X = 1      ' implicit LET
```

Key features include `PRINT`, `INPUT`, `IF`/`THEN`, `ELSE`, `FOR`/`NEXT`, `WHILE`/`WEND`, `GOTO`, `GOSUB`/`RETURN`, `DATA`/`READ`/`RESTORE`, and arrays declared with `DIM`.

## Functions and Optional Line Numbers

Functions can be declared with classic `DEF FN` syntax or with `FUNCTION`/`SUB` blocks:

```basic
DEF FNADD(A,B)=A+B
FUNCTION GREET$(NAME$)
  GREET$ = "HI " + NAME$
END FUNCTION
```

Line numbers are optional—statements can be written free‑form and execution starts at the first line:

```basic
FUNCTION ADD(A, B)
  ADD = A + B
END FUNCTION

PRINT ADD(2, 3)
```

## Graphics Features *(work in progress)*

The compiler experiments with simple graphics commands that target Kitty‑compatible terminals:

```basic
HGR2            ' switch to high‑resolution graphics
HCOLOR=3        ' select color
HPLOT 0,0 TO 100,100
```

Other commands include `HOME`, `VTAB`, `HTAB`, `TEXT`, `INVERSE`, and `NORMAL`. Output from samples such as `graphics.bas` can be viewed with `kitty +kitten icat`.

## Less Common Syntax and Instructions

Numeric literals may use hexadecimal or octal forms:

```basic
PRINT &H1A      ' hex 26
PRINT &O10      ' octal 8
```

Additional instructions extend the dialect beyond the classics:

```basic
POKE &H200, 1           ' write to memory
X = PEEK(&H200)         ' read from memory
PRINT "X"; SPC(5); X   ' insert spaces
ON N GOTO 100,200,300   ' multi-way branch
```

Use `DATE` or `DATE$` to retrieve the current date and `CLEAR` to reset program state.


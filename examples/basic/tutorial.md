# BASIC Compiler Tutorial

This guide introduces the BASIC dialect supported by `basicc`, starting with familiar features and gradually expanding into more advanced capabilities.

## Classic BASIC Essentials

Programs can begin with traditional line numbers and imperative statements:

```basic
10 PRINT "HELLO, WORLD"
20 FOR I = 1 TO 3
30   PRINT I
40 NEXT I
50 END
```

The dialect retains classic control flow constructs such as `FOR`/`NEXT` loops, `WHILE`/`WEND`, and `IF`/`THEN` branching. Arrays are declared with `DIM`, and `DATA`/`READ` supply static values.

## Functions and Optional Line Numbers

Named functions can be defined inline using `DEF FN`:

```basic
10 DEF FNADD(A,B)=A+B
20 PRINT FNADD(2,3)
```

For larger routines, `FUNCTION` and `SUB` blocks remove the need for line numbers and support structured programming:

```basic
FUNCTION ADD(A%, B%)
  ADD = A% + B%
END FUNCTION

SUB HELLO(NAME$)
  PRINT "HELLO "; NAME$
END SUB

PRINT ADD(2, 3)
HELLO "BOB"
```

Line numbers are optional throughout; when omitted, statements execute in order as written.

## Graphics (WIP)

The runtime includes preliminary text and high-resolution graphics commands:

```basic
HOME: VTAB 5: HTAB 10
INVERSE: PRINT "A": NORMAL
TEXT: HGR2
HCOLOR=2
HPLOT 3,4 TO 10,10
```

These features target terminals supporting the Kitty graphics protocol and are still evolving.

## Less Common Syntax

The dialect also accepts hexadecimal and octal literals using `&H` and `&O` prefixes:

```basic
PRINT &H1A
PRINT &O10
```

Special instructions provide low-level access and spacing control:

```basic
POKE 49152,0
PRINT PEEK(49152)
PRINT SPC(5); "INDENTED"
```

Use these carefully, as they interact directly with memory and the display.


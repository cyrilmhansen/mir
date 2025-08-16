## Statements

| Instruction | Description | Operands | Operand Types | Return |
|---|---|---|---|---|
| PRINT | print values | var | string/integer/decimal | none |
| LET | assign value | 2 | string, decimal | none |
| INC | increment variable | 1 | string | none |
| DEC | decrement variable | 1 | string | none |
| GOTO | jump to line | 1 | integer | none |
| IF | conditional | 2 | decimal, line | none |
| INPUT | read values | var | string/integer/decimal | none |
| GET | get character | 1 | string | none |
| PUT | output character | 1 | decimal | none |
| SWAP | swap variables | 2 | string, string | none |
| OPEN | open file | 2 | integer, string | none |
| CLOSE | close file | 1 | integer | none |
| PRINT# | print to file | var | integer, string/integer/decimal | none |
| INPUT# | input from file | 2 | integer, string | none |
| GET# | get from file | 2 | integer, string | none |
| PUT# | put to file | 2 | integer, decimal | none |
| DEF | define function | var | string | none |
| DATA | store data | var | string/integer/decimal | none |
| READ | read DATA | var | string/integer/decimal | none |
| RESTORE | reset DATA | 0 |  | none |
| CLEAR | clear state | 0 |  | none |
| SCREEN | set screen | 1 | integer | none |
| CLS | clear screen | 0 |  | none |
| COLOR | set color | 1 | integer | none |
| KEYOFF | disable key | 0 |  | none |
| LOCATE | set cursor | 2 | integer, integer | none |
| HTAB | horizontal tab | 1 | integer | none |
| POKE | write memory | 2 | integer, integer | none |
| HOME | home cursor | 0 |  | none |
| VTAB | vertical tab | 1 | integer | none |
| DELAY | pause | 1 | integer | none |
| BEEP | beep | 0 |  | none |
| SOUND | sound | 3 | decimal, decimal, decimal | none |
| SYSTEM | shell command | 1 | string | none |
| RANDOMIZE | seed RNG | 2 | decimal, decimal | none |
| TEXT | text mode | 0 |  | none |
| INVERSE | inverse mode | 0 |  | none |
| NORMAL | normal mode | 0 |  | none |
| HGR2 | hi-res graphics | 0 |  | none |
| HCOLOR | set graphics color | 1 | integer | none |
| HPLOT | plot points | var | decimal | none |
| MOVE | move cursor | 2 | decimal, decimal | none |
| DRAW | draw line | 2 | decimal, decimal | none |
| LINE | draw line | 4 | decimal, decimal, decimal, decimal | none |
| CIRCLE | draw circle | 3 | decimal, decimal, decimal | none |
| RECT | draw rectangle | 4 | decimal, decimal, decimal, decimal | none |
| MODE | set mode | 1 | integer | none |
| FILL | fill rectangle | 4 | decimal, decimal, decimal, decimal | none |
| END | terminate program | 0 |  | none |
| STOP | stop execution | 0 |  | none |
| REM | comment | 0 |  | none |
| DIM | dimension array | var | string, integer, integer | none |
| FOR | for loop | 4 | string, decimal, decimal, decimal | none |
| NEXT | next loop | 1 | string | none |
| WHILE | while loop | 1 | decimal | none |
| WEND | end while | 0 |  | none |
| DO | do loop | 0 |  | none |
| LOOP | loop tail | 0 |  | none |
| REPEAT | repeat loop | 0 |  | none |
| UNTIL | loop condition | 1 | decimal | none |
| GOSUB | call subroutine | 1 | integer | none |
| RETURN | return from subroutine | 0 |  | none |
| ON_GOTO | indexed GOTO | 2 | decimal, line list | none |
| ON_GOSUB | indexed GOSUB | 2 | decimal, line list | none |
| ON_ERROR | error handler | 1 | integer | none |
| RESUME | resume after error | 1 | integer | none |
| MAT | matrix operation | 1 | string | none |
| CHAIN | chain program | 1 | string | none |
| EXTERN | external call | 1 | string | none |
| CALL | call machine code | 1 | integer | none |
| EVAL | evaluate expression | 1 | string | none |

## Built-in Functions
LEN, INSTR, and ASC return integer values.

| Instruction | Description | Operands | Operand Types | Return |
|---|---|---|---|---|
| RND | random number | 1 | decimal | decimal |
| INT | integer part | 1 | decimal | integer |
| TIMER | system timer | 0 |  | integer |
| TIME | current time | 0 |  | decimal |
| DATE | current date | 0 |  | decimal |
| PEEK | read memory | 1 | integer | integer |
| EOF | end-of-file check | 1 | integer | integer |
| POS | current position | 0 |  | integer |
| ABS | absolute value | 1 | decimal/integer | decimal/integer |
| SGN | sign of number | 1 | decimal/integer | integer |
| SQR | square root | 1 | decimal | decimal |
| SIN | sine | 1 | decimal | decimal |
| COS | cosine | 1 | decimal | decimal |
| TAN | tangent | 1 | decimal | decimal |
| ATN | arctangent | 1 | decimal | decimal |
| ASIN | arcsine | 1 | decimal | decimal |
| ACOS | arccosine | 1 | decimal | decimal |
| SINH | hyperbolic sine | 1 | decimal | decimal |
| COSH | hyperbolic cosine | 1 | decimal | decimal |
| TANH | hyperbolic tangent | 1 | decimal | decimal |
| ASINH | inverse hyperbolic sine | 1 | decimal | decimal |
| ACOSH | inverse hyperbolic cosine | 1 | decimal | decimal |
| ATANH | inverse hyperbolic tangent | 1 | decimal | decimal |
| LOG | natural logarithm | 1 | decimal | decimal |
| LOG2 | log base 2 | 1 | decimal | decimal |
| LOG10 | log base 10 | 1 | decimal | decimal |
| EXP | exponential | 1 | decimal | decimal |
| FACT | factorial | 1 | integer | integer |
| PI | pi constant | 0 |  | decimal |
| LEN | string length | 1 | string | integer |
| VAL | string to number | 1 | string | decimal/integer |
| ASC | character code | 1 | string | integer |
| INSTR | substring position | 2 | string, string | integer |
| MIRCTX | get MIR context | 0 |  | integer |
| MIRMOD | create MIR module | 2 | integer, string | integer |
| MIRFUNC | create MIR function | 3 | integer, string, decimal | integer |
| MIRREG | allocate MIR register | 1 | integer | integer |
| MIRLABEL | create MIR label | 1 | integer | integer |
| MIREMIT | emit MIR instruction | 5 | integer, integer, decimal, decimal, decimal | integer |
| MIREMITLBL | emit MIR label | 2 | integer, integer | integer |
| MIRRET | emit MIR return | 2 | integer, decimal | integer |
| MIRFINISH | finalize MIR module | 1 | integer | integer |
| MIRRUN | run MIR function | 5 | integer, decimal, decimal, decimal, decimal | integer |
| MIRDUMP | dump MIR code | 1 | integer | integer |
| CHR$ | character from code | 1 | integer | string |
| UNICHAR$ | unicode character | 1 | integer | string |
| STRING$ | repeat string | 2 | integer, string | string |
| TIME$ | time as string | 0 |  | string |
| DATE$ | date as string | 0 |  | string |
| INPUT$ | input string | 0 |  | string |
| SPC | spaces | 1 | integer | string |
| SPACE$ | spaces | 1 | integer | string |
| TAB | tab to column | 1 | integer | none |
| LEFT$ | left substring | 2 | string, integer | string |
| RIGHT$ | right substring | 2 | string, integer | string |
| MID$ | mid substring | 3 | string, integer, integer | string |
| STR$ | number to string | 1 | decimal | string |
| INKEY$ | input key | 0 |  | string |
| MIRROR$ | reverse string | 1 | string | string |
| UPPER$ | upper case | 1 | string | string |
| LOWER$ | lower case | 1 | string | string |

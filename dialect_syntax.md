# BASIC Dialect Syntax

Supported BASIC instructions now include:

- `REM` comments.
- loops (`FOR`/`NEXT`, `WHILE`/`WEND`).
- arrays via `DIM` and indexed variables.
- `DATA`, `READ`, and `RESTORE` statements.
- graphics commands such as `HOME`, `VTAB`, `HTAB`, `TEXT`, `INVERSE`, `NORMAL`, `HGR2`, `HCOLOR=`, and `HPLOT`.
- memory and spacing operations like `PEEK`, `POKE`, and `SPC`.
- integer arithmetic: division (`\`) and modulo (`MOD`).
- input with `INPUT` and single-character `GET`.
- state reset via `CLEAR`.
- multi-way branching with `ON...GOTO` and `ON...GOSUB`.
- program chaining with `CHAIN <expr>` to replace the current program with another BASIC file (interpreter-only).

# MIR repository agent instructions

## Formatting
- Use tabs for indentation in source files and verify this before submitting patches to avoid unnecessary token waste.
- Run `clang-format -i` on any modified C source files before committing.

## Testing
- All new features must include accompanying unit tests; pull requests lacking
  tests will not be accepted.
- Use `make basic-test` to run unit tests for the BASIC compiler example after changes in `examples/basic` or related build rules.
- To exercise the extended BASIC features, run the `periodic.bas` sample:
  `./basic/basicc examples/basic/periodic.bas > basic/periodic.out && diff examples/basic/periodic.out basic/periodic.out`.

## BASIC compiler usage
- `examples/basic/basicc.c` accepts:
  - `-j` to execute with the JIT generator.
  - `-S` or `-c` to emit MIR text (`.mir`) and binary (`.bmir`) files.
  - `-b` to build a standalone executable.
  - `-o <path>` to set the base name for generated files.
  - Runtime helpers are implemented in `examples/basic/basic_runtime.c` and linked when building binaries.

- Supported BASIC instructions now include:
  - `REM` comments.
  - loops (`FOR`/`NEXT`, `WHILE`/`WEND`).
  - arrays via `DIM` and indexed variables.
  - `DATA`, `READ`, and `RESTORE` statements.
  - graphics commands such as `HOME`, `VTAB`, `HTAB`, `TEXT`, `INVERSE`, `NORMAL`, `HGR2`, `HCOLOR=`, and `HPLOT`.
  - memory and spacing operations like `PEEK`, `POKE`, and `SPC`.
  - integer arithmetic: division (`\\`) and modulo (`MOD`).
  - input with `INPUT` and single-character `GET`.
  - state reset via `CLEAR`.
  - multi-way branching with `ON...GOTO` and `ON...GOSUB`.

- Graphics output from BASIC samples uses the Kitty terminal protocol. View
  `.out` files in a Kitty-capable terminal or with `kitty +kitten icat`.

## Postmortem
- `basic/basicc` entered an infinite loop when parsing programs ending with `END` or `STOP`.
- The cursor in `parse_stmt` was not advanced for these keywords, causing the loop in `parse_line` to never progress.
- Advancing the cursor for `END`/`STOP` and restoring a colon check after each statement resolves the issue.
- `fleuves.bas` could not be parsed due to unhandled empty statements and missing loop generation.
- Allowing consecutive or trailing colons and emitting MIR for `FOR`/`NEXT` and `WHILE`/`WEND` enables complex BASIC programs to run.


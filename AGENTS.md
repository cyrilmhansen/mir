# MIR repository agent instructions

## Formatting
- Run `clang-format -i` on any modified C source files before committing.

## Testing
- Use `make basic-test` to run unit tests for the BASIC compiler example after changes in `examples/basic` or related build rules.
- To exercise the extended BASIC features, run the `periodic.bas` sample:
  `./build/basic/basicc examples/basic/periodic.bas > build/basic/periodic.out && diff examples/basic/periodic.out build/basic/periodic.out`.

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
  - graphics commands such as `HOME` and `VTAB`.


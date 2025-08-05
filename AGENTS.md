# MIR repository agent instructions

## Formatting
- Run `clang-format -i` on any modified C source files before committing.

## Testing
- Use `make basic-test` to run unit tests for the BASIC compiler example after changes in `examples/basic` or related build rules.

## BASIC compiler usage
- `examples/basic/basicc.c` accepts:
  - `-j` to execute with the JIT generator.
  - `-S` or `-c` to emit MIR text (`.mir`) and binary (`.bmir`) files.
  - `-o <path>` to set the base name for generated MIR files.

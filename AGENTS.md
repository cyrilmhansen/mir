# AGENTS Instructions

This repository contains **MIR**, a lightweight JIT and intermediate representation in C.

## Layout Overview
- **Core IR & Runtime:** `mir.c`, `mir.h`, `mir-gen.c`, `mir-interp.c`, memory allocation helpers, and architecture emitters such as `mir-x86_64.c` and `mir-aarch64.c`.
- **C to MIR Compiler:** `c2mir/` with per-architecture subdirectories (`x86_64/`, `aarch64/`, `ppc64/`, `s390x/`, `riscv64/`).
- **Utilities:** `mir-utils/`, `mir2c/`, `llvm2mir/` provide MIR↔text/binary conversion and integrations.
- **Tests & Benchmarks:** `mir-tests/`, `c-tests/`, `adt-tests/`, `c-benchmarks/` show how to construct and execute MIR modules.
- **Docs & Build Files:** `README.md`, `MIR.md`, `INSTALL.md`, `HOW-TO-PORT-MIR.md`, `GNUmakefile`, and `CMakeLists.txt`.

## Development Notes
- Start with examples in the test directories to learn MIR usage.
- Most APIs operate through a `MIR_context` handle.
- When generating machine code, see the optimization pipeline at the top of `mir-gen.c` and inspect architecture-specific generators like `mir-gen-x86_64.c`.

## Testing
Run `make test` from the repository root after making changes.

#!/usr/bin/env bash
# Run BASIC benchmarks in JIT and compiled modes.
set -e
ROOT="$(realpath "$(dirname "$0")/../../")"
BUILD_DIR="$ROOT"
BASICC="$BUILD_DIR/basic/basicc"

# Build basicc if necessary
if [ ! -x "$BASICC" ]; then
  make -C "$ROOT" "$BASICC"
fi

run_bench() {
  local name="$1"
  local file="$ROOT/examples/basic/${name}.bas"
  echo "== ${name} =="
  echo "-- JIT --"
  time -p "$BASICC" -j "$file" >/dev/null
  echo "-- Compiled --"
  time -p "$BASICC" -b -o "$BUILD_DIR/basic/${name}-bench" "$file" >/dev/null
  time -p "$BUILD_DIR/basic/${name}-bench" >/dev/null
}

run_bench sieve
run_bench fib

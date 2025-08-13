#!/usr/bin/env bash
# Run selected Minimal BASIC test programs with basicc.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
BASICC="${1:-$ROOT/basic/basicc}"

# Selected test cases from the NBS Minimal BASIC suite.
CASES=(
  P001.BAS
  P049.BAS
  P067.BAS
  P173.BAS
  P174.BAS
  ELSEIF.BAS
)

# Programs currently expected to fail.
XFAIL=(P173.BAS)

run_case() {
  local mode="$1"; shift
  local file="$1"; shift
  local src="$ROOT/examples/basic/test/mbasic-nbs/$file"
  local -a flags=()
  [[ "$mode" == compiled ]] && flags+=(-j)
  echo "Running $file ($mode)"
  local out
  out="$("$BASICC" "${flags[@]}" "$src" 2>&1 || true)"
  local ok=0
  case "$file" in
    P067.BAS)
      if echo "$out" | grep -q '\*\*\* EXCEPTION SHOULD OCCUR NOW \*\*\*' \
         && ! echo "$out" | grep -q '\*\*\* TEST FAILS \*\*\*'; then
        ok=1
      fi
      ;;
    *)
      if echo "$out" | grep -q '\*\*\* TEST PASSED'; then
        ok=1
      fi
      ;;
  esac
  if [[ $ok -eq 1 ]]; then
    echo "$file ($mode) OK"
  else
    if [[ " ${XFAIL[*]} " == *" $file "* ]]; then
      echo "$file ($mode) expected failure"
    else
      echo "$file ($mode) FAILED"
      return 1
    fi
  fi
}

run_mode() {
  local mode="$1"
  local status=0
  for file in "${CASES[@]}"; do
    if ! run_case "$mode" "$file"; then
      status=1
    fi
  done
  return $status
}

# Run both interpreted and JIT-compiled modes.
run_mode interpreted
run_mode compiled

# Verify INC/DEC support
for mode in interpreted compiled; do
  flags=()
  [[ "$mode" == compiled ]] && flags+=(-j)
  echo "Running incdec.bas ($mode)"
  out="$ROOT/basic/incdec.$mode.out"
  "$BASICC" "${flags[@]}" "$ROOT/examples/basic/test/incdec.bas" > "$out"
  diff "$ROOT/examples/basic/test/incdec.out" "$out"
  rm -f "$out"
  echo "incdec.bas ($mode) OK"
done


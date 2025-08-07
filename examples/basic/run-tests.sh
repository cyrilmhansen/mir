#!/usr/bin/env bash
# Run BASIC compiler example tests.
set -eu
ROOT="$(cd "$(dirname "$0")/../../" && pwd)"
BASICC="$ROOT/basic/basicc"
# Build basicc if missing
if [ ! -x "$BASICC" ]; then
	(cd "$ROOT" && make basic/basicc)
fi

run_test() {
        local name="$1"
        local in_file="$ROOT/examples/basic/$name.in"
        local src="$ROOT/examples/basic/$name.bas"
        local exp="$ROOT/examples/basic/$name.out"
        local out="$ROOT/basic/$name.out"
        if [ -f "$in_file" ]; then
                "$BASICC" "$src" < "$in_file" > "$out"
        else
                "$BASICC" "$src" > "$out"
        fi
        if [ "$name" = "circle" ] || [ "$name" = "box" ]; then
                grep -a "$(echo "$name" | tr a-z A-Z)" "$out" > "$out.filtered" || true
                mv "$out.filtered" "$out"
        fi
        diff "$exp" "$out"
}

for t in hello relop adder guess string strfuncs instr gosub funcproc graphics readhplot circle box sudoku; do
	echo "Running $t"
	run_test "$t"
	echo "$t OK"
done

echo "Running fleuves (no explain)"
timeout 10 "$BASICC" "$ROOT/examples/basic/fleuves.bas" < "$ROOT/examples/basic/fleuves.in" >/dev/null
echo "fleuves (no explain) done"

echo "Running fleuves (with explain)"
timeout 10 "$BASICC" "$ROOT/examples/basic/fleuves.bas" < "$ROOT/examples/basic/fleuves_explain.in" >/dev/null
echo "fleuves (with explain) done"

echo "Running repl LOAD"
printf 'LOAD %s\nRUN\nQUIT\n' "$ROOT/examples/basic/hello.bas" | "$BASICC" > "$ROOT/basic/repl-load.out"
diff "$ROOT/examples/basic/repl-load.out" "$ROOT/basic/repl-load.out"
echo "repl LOAD done"


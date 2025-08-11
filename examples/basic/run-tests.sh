#!/usr/bin/env bash
# Run BASIC compiler example tests.
set -eu
ROOT="$(cd "$(dirname "$0")/../../" && pwd)"

# Determine BASIC compiler binaries to test. If none are supplied as arguments,
# test the double and long double variants built by the makefile.
if [ "$#" -gt 0 ]; then
        BASICCS=("$@")
else
        BASICCS=(
                "$ROOT/basic/basicc"
                "$ROOT/basic/basicc-ld"
        )
fi

run_tests() {
        BASICC="$1"

        # Build compiler if missing
        if [ ! -x "$BASICC" ]; then
                (cd "$ROOT" && make "basic/$(basename "$BASICC")")
        fi

        run_test() {
                local name="$1"
                local in_file="$ROOT/examples/basic/$name.in"
                local src="$ROOT/examples/basic/$name.bas"
                local exp="$ROOT/examples/basic/$name.out"
                local out="$ROOT/basic/$name.out"
                if [ -f "$in_file" ]; then
                        "$BASICC" "$src" < "$in_file" > "$out"
                elif [ "$name" = "mandelbrot" ]; then
                        timeout 0.6 "$BASICC" "$src" | head -n 200 > "$out" || true
                else
                        "$BASICC" "$src" > "$out"
                fi
                if [ "$name" = "circle" ] || [ "$name" = "box" ]; then
                        grep -ao "$(echo "$name" | tr a-z A-Z)" "$out" > "$out.filtered" || true
                        mv "$out.filtered" "$out"
                fi
                if [ "$name" = "datediff" ]; then
                        local y m d doy total
                        y=$(sed -n '1p' "$in_file")
                        m=$(sed -n '2p' "$in_file")
                        d=$(sed -n '3p' "$in_file")
                        set -- $(python3 - "$y" "$m" "$d" <<'PY'
import sys, datetime
y, m, d = map(int, sys.argv[1:])
dt = datetime.date(y, m, d)
print(dt.timetuple().tm_yday, datetime.date(y, 12, 31).timetuple().tm_yday)
PY
)
                        doy=$1
                        total=$2
                        exp="$out.expected"
                        printf "Year: \nMonth: \nDay: \nDays since Jan 1: %d\nDays until Dec 31: %d\n" $((doy-1)) $((total-doy)) > "$exp"
                        diff "$exp" "$out"
                        rm -f "$exp"
                elif [ "$name" = "pi" ]; then
                        grep -q '^PI=3.14159265358979' "$out"
                        grep -q '^TIME=' "$out" >/dev/null
                else
                        diff "$exp" "$out"
                fi
        }

        echo "Running hcolor_test"
        "$ROOT/basic/hcolor_test" > "$ROOT/basic/hcolor_test.out"
        diff "$ROOT/examples/basic/hcolor_test.out" "$ROOT/basic/hcolor_test.out"
        echo "hcolor_test OK"

        echo "Building extlib"
        if [[ "$BASICC" == *-ld ]]; then
                cc -shared -fPIC -Wall -Wextra -DBASIC_USE_LONG_DOUBLE -I"$ROOT/examples/basic" \
                        "$ROOT/examples/basic/extlib.c" -o "$ROOT/basic/libextlib.so"
        else
                cc -shared -fPIC -Wall -Wextra -I"$ROOT/examples/basic" \
                        "$ROOT/examples/basic/extlib.c" -o "$ROOT/basic/libextlib.so"
        fi
        echo "Running extern"
        LD_PRELOAD="$ROOT/basic/libextlib.so" "$BASICC" "$ROOT/examples/basic/extern.bas" \
                > "$ROOT/basic/extern.out"
        diff "$ROOT/examples/basic/extern.out" "$ROOT/basic/extern.out"
        echo "extern OK"

        for t in hello relop adder string strfuncs instr gosub on funcproc graphics hplot_bounds readhplot restore hgr2reset circle box sudoku array_oob_read array_oob_write dim_expr pi baseconv mir_demo datediff date rnd_noarg hexoct; do
                echo "Running $t"
                run_test "$t"
                echo "$t OK"
        done

        echo "Running hello (no line tracking)"
        "$BASICC" --no-line-tracking "$ROOT/examples/basic/hello.bas" > "$ROOT/basic/hello-no-line.out"
        diff "$ROOT/examples/basic/hello.out" "$ROOT/basic/hello-no-line.out"
        echo "hello (no line tracking) OK"

        echo "Running resume (expect error)"
        if "$BASICC" --no-line-tracking "$ROOT/examples/basic/resume.bas" >/dev/null 2> "$ROOT/basic/resume.err"; then
                echo "resume should have failed"
                exit 1
        fi
        grep -q "line tracking" "$ROOT/basic/resume.err"
        echo "resume error OK"

echo "Running dim expression (expect error)"
if "$BASICC" "$ROOT/examples/basic/dim_expr_error.bas" >/dev/null 2> "$ROOT/basic/dim_expr_error.err"; then
echo "dim expression should have failed"
exit 1
fi
grep -q "expected integer" "$ROOT/basic/dim_expr_error.err"
echo "dim expression error OK"

echo "Running line number float (expect error)"
if "$BASICC" "$ROOT/examples/basic/line_number_float.bas" >/dev/null 2> "$ROOT/basic/line_number_float.err"; then
echo "line number float should have failed"
exit 1
fi
grep -q "expected integer" "$ROOT/basic/line_number_float.err"
echo "line number float error OK"

echo "Running print expression (expect error)"
"$BASICC" "$ROOT/examples/basic/print_expr_error.bas" >/dev/null \
        2> "$ROOT/basic/print_expr_error.err" || true
if ! grep -q "parse error at line 10" "$ROOT/basic/print_expr_error.err"; then
        echo "print expression should have failed"
        exit 1
fi
echo "print expression error OK"

        echo "Running repl LOAD"
        printf 'LOAD %s\nRUN\nQUIT\n' "$ROOT/examples/basic/hello.bas" | "$BASICC" > "$ROOT/basic/repl-load.out"
        diff "$ROOT/examples/basic/repl-load.out" "$ROOT/basic/repl-load.out"
        echo "repl LOAD done"

        echo "Running repl LIST"
        printf 'LOAD %s\nLIST\nQUIT\n' "$ROOT/examples/basic/funcproc.bas" | "$BASICC" > "$ROOT/basic/repl-list.out"
        diff "$ROOT/examples/basic/repl-list.out" "$ROOT/basic/repl-list.out"
        echo "repl LIST done"

        echo "Running repl CODE"
        printf '10 PRINT "HI"\nCOMPILE CODE repl-code.bin\nQUIT\n' | "$BASICC" > "$ROOT/basic/repl-code.out"
        diff "$ROOT/examples/basic/repl-code.out" "$ROOT/basic/repl-code.out"
        test -s repl-code.bin
        rm -f repl-code.bin
        echo "repl CODE done"

        echo "Running repl PROFILING"
        printf '10 DEF FNA(X)=X+1\n20 PRINT FNA(1)\n30 PRINT FNA(2)\n40 END\nRUN PROFILING\nQUIT\n' | "$BASICC" > "$ROOT/basic/repl-prof.out"
        grep -q 'line 20: count 1' "$ROOT/basic/repl-prof.out"
        grep -q 'func FNA: count 2' "$ROOT/basic/repl-prof.out"
        echo "repl PROFILING done"

        echo "Running repl PROFILING funcproc"
        printf 'LOAD %s\nRUN PROFILING\nQUIT\n' "$ROOT/examples/basic/funcproc.bas" | "$BASICC" > "$ROOT/basic/repl-prof-funcproc.out"
        grep -q 'func ADD: count 1' "$ROOT/basic/repl-prof-funcproc.out"
        grep -q 'func HELLO: count 2' "$ROOT/basic/repl-prof-funcproc.out"
        echo "repl PROFILING funcproc done"
}

for cc in "${BASICCS[@]}"; do
        echo "=== Testing with $(basename "$cc") ==="
        run_tests "$cc"
done


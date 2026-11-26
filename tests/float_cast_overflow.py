#!/usr/bin/env python3
"""Check --ub-float-cast-overflow-check across C and raw LLVM IR."""
from pathlib import Path
import subprocess
import sys
import tempfile

frontend, binary, clang = sys.argv[1:]
command = [sys.executable, frontend, "--bin", binary, "--clang", clang]
flag = "--ub-float-cast-overflow-check"


def check(args, code, message):
    proc = subprocess.run(args, capture_output=True, text=True, timeout=30)
    output = proc.stdout + proc.stderr
    assert proc.returncode == code, output
    assert message in output, output
    return output


with tempfile.TemporaryDirectory() as directory:
    root = Path(directory)
    source = root / "main.c"
    ir = root / "main.ll"

    # C11 6.3.1.4p1 valid boundaries: negative fractions truncating to 0,
    # INT_MIN - 0.5 truncating to INT_MIN, exact limits for float/double/long double.
    source.write_text('''
#include <stdint.h>
extern float __VERIFIER_nondet_float(void);
extern double __VERIFIER_nondet_double(void);
extern long double __VERIFIER_nondet_long_double(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  float a = __VERIFIER_nondet_float();
  __VERIFIER_assume(a > -1.0f && a <= 0.0f);
  unsigned u = (unsigned)a;
  __VERIFIER_assert(u == 0u);

  double d = __VERIFIER_nondet_double();
  __VERIFIER_assume(d == -2147483648.5);
  int i = (int)d;
  __VERIFIER_assert(i == -2147483647 - 1);

  float fmin = __VERIFIER_nondet_float();
  __VERIFIER_assume(fmin == -2147483648.0f);
  int fi = (int)fmin;
  __VERIFIER_assert(fi == -2147483647 - 1);

  long double ld = __VERIFIER_nondet_long_double();
  __VERIFIER_assume(ld == -9223372036854775808.0L);
  int64_t ldi = (int64_t)ld;
  __VERIFIER_assert(ldi == INT64_MIN);
  return 0;
}
''')
    for extra in ([flag], [flag, "--fp2bv"], [flag, "--incremental-solver"]):
        check(command + [str(source)] + extra, 0, "VERIFICATION SUCCESSFUL")

    # Out-of-range cases: -1.0f -> unsigned, 2^31 -> int32, NaN, +inf, -inf, long double -> int64.
    fail_cases = [
        ("unsigned", "float", "x == -1.0f"),
        ("int", "double", "x == -2147483649.0"),
        ("int", "float", "x == 2147483648.0f"),
        ("unsigned", "double", "x == 4294967296.0"),
        ("int", "float", "x != x"),
        ("int", "double", "x == __builtin_inf()"),
        ("unsigned", "double", "x == -__builtin_inf()"),
        ("long long", "long double", "x == 9223372036854775808.0L"),
    ]
    for dst_ty, src_ty, cond in fail_cases:
        nondet = {"float": "float", "double": "double", "long double": "long_double"}[src_ty]
        source.write_text(f'''
extern {src_ty} __VERIFIER_nondet_{nondet}(void);
extern void __VERIFIER_assume(int);
int main(void) {{
  {src_ty} x = __VERIFIER_nondet_{nondet}();
  __VERIFIER_assume({cond});
  volatile {dst_ty} r = ({dst_ty})x;
  (void)r;
  return 0;
}}
''')
        out = check(command + [str(source), flag], 1, "main.float-cast-overflow.1")
        assert "main.float-cast-overflow.2" not in out, out
        check(command + [str(source), flag, "--fp2bv"], 1, "main.float-cast-overflow.1")
        check(command + [str(source)], 0, "VERIFICATION SUCCESSFUL")
        check(command + [str(source), flag, flag + "=false"], 0, "VERIFICATION SUCCESSFUL")
        check(command + [str(source), flag, "--no-assertions"], 1, "main.float-cast-overflow.1")

    # Excluded under --property unreach-call and --property no-overflow (must NOT prune reach_error).
    source.write_text('''
extern void __assert_fail(const char *, const char *, unsigned, const char *);
void reach_error(void) { __assert_fail("0", "main.c", 2, "reach_error"); }
extern float __VERIFIER_nondet_float(void);
int main(void) {
  float x = __VERIFIER_nondet_float();
  volatile unsigned u = (unsigned)x;
  (void)u;
  if (x == -1.0f)
    reach_error();
  return 0;
}
''')
    check(command + [str(source), flag, "--property", "unreach-call"], 1, "VERIFICATION FAILED")
    source.write_text("int main(void) { volatile int x = (int)1e30; (void)x; return 0; }\n")
    check(command + [str(source), flag, "--property", "no-overflow"], 0, "VERIFICATION SUCCESSFUL")

    # Pruning an invalid cast must preserve earlier failures in multi-property mode.
    source.write_text('''
extern void __VERIFIER_assert(int);
int main(void) {
  __VERIFIER_assert(0);
  volatile int x = (int)1e30;
  (void)x;
  return 0;
}
''')
    out = check(command + [str(source), flag, "--multi-property"], 1, "main.assertion.1")
    assert "main.float-cast-overflow.1" in out, out

    # Raw LLVM IR: scalar widths (including i1 and i128 where 2^128 exceeds float max) and vector lanes.
    ir.write_text('''
define i32 @main(float %x) {
  %r = fptoui float %x to i128
  ret i32 0
}
''')
    # float -> i128 fails on negative/NaN/+inf, succeeds for finite nonnegative floats.
    check([binary, str(ir), flag], 1, "main.float-cast-overflow.1")
    ir.write_text('''
declare void @__VERIFIER_assume(i1)
define i32 @main(float %x) {
  %nonneg = fcmp oge float %x, 0.0
  %fin = fcmp olt float %x, 0x7FF0000000000000
  %ok = and i1 %nonneg, %fin
  call void @__VERIFIER_assume(i1 %ok)
  %r = fptoui float %x to i128
  ret i32 0
}
''')
    check([binary, str(ir), flag], 0, "VERIFICATION SUCCESSFUL")

    # Raw IR: signed i1 accepts -1.5 (truncates to -1) and rejects -2.0 and 1.0.
    ir.write_text('''
define i32 @main() {
  %a = fptosi double -1.5 to i1
  %b = fptosi double 0.75 to i1
  ret i32 0
}
''')
    check([binary, str(ir), flag], 0, "VERIFICATION SUCCESSFUL")
    ir.write_text('''
define i32 @main() {
  %a = fptosi double -2.0 to i1
  ret i32 0
}
''')
    check([binary, str(ir), flag], 1, "main.float-cast-overflow.1")

    # Raw IR: vector lanes checked before scalarization (lane 1 overflows).
    ir.write_text('''
define i32 @main(float %x) {
  %v0 = insertelement <2 x float> <float 0.0, float -1.0>, float %x, i32 0
  %r = fptoui <2 x float> %v0 to <2 x i32>
  ret i32 0
}
''')
    check([binary, str(ir), flag], 1, "main.float-cast-overflow.1")

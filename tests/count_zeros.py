#!/usr/bin/env python3
"""Check count-zero encodings and source/IR builtin preconditions."""
from pathlib import Path
import subprocess
import sys
import tempfile

frontend, binary, clang = sys.argv[1:]
command = [sys.executable, frontend, "--bin", binary, "--clang", clang]
flag = "--ub-count-zero-check"


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
    # Compare dynamic counts with independent shift loops over every nonzero input.
    source.write_text('''
extern unsigned long long __VERIFIER_nondet_ulonglong(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned long long x = __VERIFIER_nondet_ulonglong();
  __VERIFIER_assume(x != 0);
  unsigned long long y = x;
  unsigned trailing = 0, bits = 0;
  while (!(y & 1)) { y >>= 1; ++trailing; }
  y = x;
  while (y) { y >>= 1; ++bits; }
  __VERIFIER_assert(__builtin_ctzll(x) == trailing);
  __VERIFIER_assert(__builtin_clzll(x) == 64 - bits);
}
''')
    for flags in ([], [flag], [flag, "--incremental-solver"]):
        check(command + [str(source), "--unwind", "65"] + flags, 0, "VERIFICATION SUCCESSFUL")

    for builtin in ("clz", "ctz", "clzll", "ctzll"):
        source.write_text(f"int main(int x, char **p) {{ int unused = __builtin_{builtin}(x); }}\n")
        for flags in ([flag], [flag, "--no-assertions"], [flag, "--incremental-solver"],
                      [flag, "--incremental-bmc"]):
            out = check(command + [str(source)] + flags, 1, "main.count-zero.1")
            assert "main.count-zero.2" not in out, out
        check(command + [str(source)], 0, "VERIFICATION SUCCESSFUL")
        check(command + [str(source), flag, flag + "=false"], 0, "VERIFICATION SUCCESSFUL")
        source.write_text(f"int main(void) {{ int unused = __builtin_{builtin}(0); }}\n")
        check(command + [str(source), flag], 1, "main.count-zero.1")

    # The source builtin is UB even when a later select masks its result.
    source.write_text("int main(int x, char **p) { int r = __builtin_ctz(x); return x ? r : 0; }\n")
    check(command + [str(source), flag], 1, "main.count-zero.1")
    # Defined fallbacks and ffs must accept zero despite poison-on-zero IR lowering.
    source.write_text('''
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);
int main(void) {
 unsigned x = __VERIFIER_nondet_uint();
 int a = __builtin_ctzg(x, 91), b = __builtin_clzg(x, 92);
 __VERIFIER_assert(x ? a == __builtin_ctz(x) : a == 91);
 __VERIFIER_assert(x ? b == __builtin_clz(x) : b == 92);
 __VERIFIER_assert(x ? __builtin_ffs(x) == a + 1 : __builtin_ffs(x) == 0);
}
''')
    check(command + [str(source), flag], 0, "VERIFICATION SUCCESSFUL")
    for prop in ("unreach-call", "no-overflow"):
        source.write_text("int main(void) { int unused = __builtin_ctz(0); }\n")
        check(command + [str(source), flag, "--property", prop], 0, "VERIFICATION SUCCESSFUL")
    source.write_text("int main(void) { __builtin_assume(0); }\n")
    check(command + [str(source), flag], 0, "VERIFICATION SUCCESSFUL")
    source.write_text("int main(void) { int a[1024]; int d = 1; "
                      "for (int i = 0; i < 1024; ++i) a[i] = __builtin_ctz(d); }\n")
    prepared = check(command + [str(source), flag, "--dump-ir-only"], 0, "define")
    assert "__vereda_check_count_zero" not in prepared, prepared

    for width in (1, 7, 8, 16, 32, 64, 128):
        for intrinsic in ("ctlz", "cttz"):
            for poison in ("true", "false"):
                ir.write_text(f'''declare i{width} @llvm.{intrinsic}.i{width}(i{width}, i1 immarg)
define i32 @main(i{width} %x) {{
 %r = call i{width} @llvm.{intrinsic}.i{width}(i{width} %x, i1 {poison})
 ret i32 0
}}
''')
                check([binary, str(ir), flag], 1 if poison == "true" else 0,
                      "main.count-zero.1" if poison == "true" else "VERIFICATION SUCCESSFUL")
            # Keep the dynamic result live and validate the defined-zero value.
            ir.write_text(f'''declare i{width} @llvm.{intrinsic}.i{width}(i{width}, i1 immarg)
declare void @__VERIFIER_assert(i1)
define i32 @main(i{width} %x) {{
 %r = call i{width} @llvm.{intrinsic}.i{width}(i{width} %x, i1 false)
 %zero = icmp eq i{width} %x, 0
 %expected = icmp eq i{width} %r, {width % (1 << width)}
 %nonzero = xor i1 %zero, true
 %ok = or i1 %nonzero, %expected
 call void @__VERIFIER_assert(i1 %ok)
 ret i32 0
}}
''')
            check([binary, str(ir), flag], 0, "VERIFICATION SUCCESSFUL")

    # Check every vector lane before scalarization, even when the result is dead.
    ir.write_text('''declare <2 x i32> @llvm.cttz.v2i32(<2 x i32>, i1 immarg)
define i32 @main(i32 %x) {
 %v = insertelement <2 x i32> <i32 1, i32 0>, i32 %x, i32 0
 %r = call <2 x i32> @llvm.cttz.v2i32(<2 x i32> %v, i1 true)
 ret i32 0
}
''')
    check([binary, str(ir), flag], 1, "main.count-zero.1")
    ir.write_text('''declare <2 x i8> @llvm.ctlz.v2i8(<2 x i8>, i1 immarg)
declare void @__VERIFIER_assert(i1)
define i32 @main(i8 %x) {
 %v = insertelement <2 x i8> zeroinitializer, i8 %x, i32 0
 %r = call <2 x i8> @llvm.ctlz.v2i8(<2 x i8> %v, i1 false)
 %first = extractelement <2 x i8> %r, i32 0
 %second = extractelement <2 x i8> %r, i32 1
 %nonzero = icmp ne i8 %x, 0
 %expected = icmp eq i8 %first, 8
 %ok = or i1 %nonzero, %expected
 %other = icmp eq i8 %second, 8
 call void @__VERIFIER_assert(i1 %ok)
 call void @__VERIFIER_assert(i1 %other)
 ret i32 0
}
''')
    check([binary, str(ir), flag], 0, "VERIFICATION SUCCESSFUL")
    # Pruning an invalid builtin must preserve earlier failures in multi-property mode.
    source.write_text("extern void __VERIFIER_assert(int);\n"
                      "int main(void) { __VERIFIER_assert(0); int x = __builtin_clz(0); }\n")
    out = check(command + [str(source), flag, "--multi-property"], 1, "main.assertion.1")
    assert "main.count-zero.1" in out, out

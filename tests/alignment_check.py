#!/usr/bin/env python3
"""Check --ub-alignment-check on aligned/misaligned C and IR accesses."""
from pathlib import Path
import subprocess
import sys
import tempfile

frontend, binary, clang = sys.argv[1:]
command = [sys.executable, frontend, "--bin", binary, "--clang", clang]
flag = "--ub-alignment-check"


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

    # Legal accesses: aligned array, packed struct, char*, memcpy, cast without deref.
    source.write_text('''
#include <stdint.h>
#include <string.h>
struct __attribute__((packed)) P { char c; int x; };
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  int a[4] = {1, 2, 3, 4};
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 4);
  int v = a[i];
  __VERIFIER_assert(v >= 1 && v <= 4);

  struct P p = {'a', 42};
  __VERIFIER_assert(p.x == 42);

  char buf[8] = {0};
  int *unaligned_unused = (int *)(buf + 1); // cast without dereference is legal
  (void)unaligned_unused;
  int copied = 0;
  memcpy(&copied, buf + 1, sizeof(copied));
  __VERIFIER_assert(copied == 0);
  char b = buf[3];
  __VERIFIER_assert(b == 0);
  return 0;
}
''')
    for extra in ([flag], [flag, "-m32"], [flag, "--incremental-solver"]):
        check(command + [str(source)] + extra, 0, "VERIFICATION SUCCESSFUL")

    # Misaligned constant offset store/load fails with alignment property id.
    source.write_text('''
#include <stdint.h>
int main(void) {
  _Alignas(8) char buf[16] = {0};
  int *p = (int *)(buf + 1);
  *p = 7;
  return 0;
}
''')
    check(command + [str(source), flag], 1, "main.alignment.1")
    check(command + [str(source), flag, "-m32"], 1, "main.alignment.1")
    check(command + [str(source), flag, "--no-assertions"], 1, "main.alignment.1")
    check(command + [str(source)], 0, "VERIFICATION SUCCESSFUL")
    check(command + [str(source), flag, flag + "=false"], 0, "VERIFICATION SUCCESSFUL")

    # Symbolic offset: misaligned when k % 4 != 0, safe when k % 4 == 0.
    source.write_text('''
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
int main(void) {
  _Alignas(8) char buf[16] = {0};
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k < 8);
  int *p = (int *)(buf + k);
  return *p;
}
''')
    check(command + [str(source), flag], 1, "main.alignment.1")

    source.write_text('''
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
int main(void) {
  _Alignas(8) char buf[16] = {0};
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k < 8 && (k & 3u) == 0u);
  int *p = (int *)(buf + k);
  return *p;
}
''')
    check(command + [str(source), flag], 0, "VERIFICATION SUCCESSFUL")

    # 1-byte aligned local cast to int* can be misaligned.
    source.write_text('''
int main(void) {
  char c[4] = {0, 0, 0, 0};
  int *p = (int *)c;
  return *p;
}
''')
    check(command + [str(source), flag], 1, "main.alignment.1")

    # Excluded under --property unreach-call and --property no-overflow (does not prune).
    source.write_text('''
extern void __assert_fail(const char *, const char *, unsigned, const char *);
void reach_error(void) { __assert_fail("0", "main.c", 2, "reach_error"); }
int main(void) {
  _Alignas(8) char buf[16] = {0};
  int *p = (int *)(buf + 1);
  *p = 1;
  reach_error();
  return 0;
}
''')
    check(command + [str(source), flag, "--property", "unreach-call"], 1, "VERIFICATION FAILED")
    check(command + [str(source), flag, "--property", "no-overflow"], 0, "VERIFICATION SUCCESSFUL")

    # Raw LLVM IR: explicit align 1 passes, align 4 at offset 2 fails.
    ir.write_text('''
define i32 @main() {
  %a = alloca [8 x i8], align 8
  %p = getelementptr i8, ptr %a, i64 2
  %v = load i32, ptr %p, align 1
  ret i32 %v
}
''')
    check([binary, str(ir), flag], 0, "VERIFICATION SUCCESSFUL")
    ir.write_text('''
define i32 @main() {
  %a = alloca [8 x i8], align 8
  %p = getelementptr i8, ptr %a, i64 2
  %v = load i32, ptr %p, align 4
  ret i32 %v
}
''')
    check([binary, str(ir), flag], 1, "main.alignment.1")

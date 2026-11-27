#!/usr/bin/env python3
"""Check --ub-uninit-check across local, struct, array, memcpy, and heap reads."""
from pathlib import Path
import subprocess
import sys
import tempfile

frontend, binary, clang = sys.argv[1:]
command = [sys.executable, frontend, "--bin", binary, "--clang", clang]
flag = "--ub-uninit-check"


def check(args, code, message):
    proc = subprocess.run(args, capture_output=True, text=True, timeout=30)
    output = proc.stdout + proc.stderr
    assert proc.returncode == code, output
    assert message in output, output
    return output


with tempfile.TemporaryDirectory() as directory:
    root = Path(directory)
    source = root / "main.c"

    # 1. Direct uninitialised local read fails with --ub-uninit-check, passes when off.
    source.write_text('''
int main(void) {
  int x;
  return x;
}
''')
    check(command + [str(source), flag], 1, "main.uninitialised-read.1")
    check(command + [str(source), flag, "--no-assertions"], 1, "main.uninitialised-read.1")
    check(command + [str(source)], 0, "VERIFICATION SUCCESSFUL")
    check(command + [str(source), flag, flag + "=false"], 0, "VERIFICATION SUCCESSFUL")

    # 2. Conditional initialisation: safe when read is guarded by same condition, fails otherwise.
    source.write_text('''
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int c = __VERIFIER_nondet_int();
  int x;
  if (c) x = 42;
  if (c) return x;
  return 0;
}
''')
    check(command + [str(source), flag], 0, "VERIFICATION SUCCESSFUL")

    source.write_text('''
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int c = __VERIFIER_nondet_int();
  int x;
  if (c) x = 42;
  return x;
}
''')
    check(command + [str(source), flag], 1, "main.uninitialised-read.1")

    # 3. Partial struct write + bytewise memcpy preserving padding without false positive.
    source.write_text('''
#include <string.h>
struct S { char c; int x; int y; };
extern void __VERIFIER_assert(int);
int main(void) {
  struct S a, b;
  a.c = 1;
  a.x = 7;
  memcpy(&b, &a, sizeof(a));
  __VERIFIER_assert(b.c == 1 && b.x == 7);
  return 0;
}
''')
    check(command + [str(source), flag], 0, "VERIFICATION SUCCESSFUL")

    source.write_text('''
#include <string.h>
struct S { char c; int x; int y; };
int main(void) {
  struct S a, b;
  a.c = 1;
  a.x = 7;
  memcpy(&b, &a, sizeof(a));
  return b.y;
}
''')
    check(command + [str(source), flag], 1, "main.uninitialised-read.1")

    # 4. Heap: malloc is uninitialised, calloc is initialised, realloc preserves prefix only.
    source.write_text('''
#include <stdlib.h>
int main(void) {
  int *p = malloc(sizeof(int));
  int v = *p;
  free(p);
  return v;
}
''')
    check(command + [str(source), flag, "--force-malloc-success"], 1, "main.uninitialised-read.1")

    source.write_text('''
#include <stdlib.h>
extern void __VERIFIER_assert(int);
int main(void) {
  int *p = calloc(2, sizeof(int));
  __VERIFIER_assert(p[0] == 0 && p[1] == 0);
  free(p);
  return 0;
}
''')
    check(command + [str(source), flag, "--force-malloc-success"], 0, "VERIFICATION SUCCESSFUL")

    source.write_text('''
#include <stdlib.h>
int main(void) {
  int *p = malloc(sizeof(int));
  p[0] = 11;
  int *q = realloc(p, 2 * sizeof(int));
  int v = q[1]; // growth byte is uninitialised
  free(q);
  return v;
}
''')
    check(command + [str(source), flag, "--force-malloc-success"], 1, "main.uninitialised-read.1")

    # 5. Genuinely nondet inputs and globals remain valid.
    source.write_text('''
extern int __VERIFIER_nondet_int(void);
static int g;
int main(void) {
  int x = __VERIFIER_nondet_int();
  return x + g;
}
''')
    check(command + [str(source), flag], 0, "VERIFICATION SUCCESSFUL")

    # 6. Excluded under --property unreach-call and --property no-overflow (does not prune).
    source.write_text('''
extern void __assert_fail(const char *, const char *, unsigned, const char *);
void reach_error(void) { __assert_fail("0", "main.c", 2, "reach_error"); }
int main(void) {
  int x;
  int y = x;
  (void)y;
  reach_error();
  return 0;
}
''')
    check(command + [str(source), flag, "--property", "unreach-call"], 1, "VERIFICATION FAILED")
    check(command + [str(source), flag, "--property", "no-overflow"], 0, "VERIFICATION SUCCESSFUL")

    # 7. Atomic read-modify-write and compare-exchange must check initialisation before updating.
    source.write_text('''
#include <stdatomic.h>
int main(void) {
  _Atomic int x;
  return atomic_fetch_add(&x, 1);
}
''')
    check(command + [str(source), flag], 1, "main.uninitialised-read.1")

    source.write_text('''
#include <stdatomic.h>
int main(void) {
  _Atomic int x;
  int expected = 0;
  return atomic_compare_exchange_strong(&x, &expected, 1);
}
''')
    check(command + [str(source), flag], 1, "main.uninitialised-read.1")

    # 8. Whole-object memset after a symbolic store must overwrite stale uninit flags in st.init.
    source.write_text('''
#include <stdlib.h>
#include <string.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
int main(void) {
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 1024);
  int *p = malloc(1024 * sizeof(int));
  p[i] = 1;
  memset(p, 0, 1024 * sizeof(int));
  int v = p[(i + 1) % 1024];
  free(p);
  return v;
}
''')
    check(command + [str(source), flag, "--force-malloc-success"], 0, "VERIFICATION SUCCESSFUL")

    # Copying a partly initialised struct reads no value: clang passes, returns and
    # assigns it as integer loads and stores. Only a later read of the member
    # nothing set fails, after the copy has carried its bytes along.
    source.write_text('''
struct S { int a; int b; };
__attribute__((noinline)) int f(struct S s) { return s.a; }
int main(void) { struct S s; s.a = 1; return f(s); }
''')
    check(command + [str(source), flag], 0, "VERIFICATION SUCCESSFUL")
    source.write_text('''
struct S { int a; int b; };
__attribute__((noinline)) int f(struct S s) { return s.b; }
int main(void) { struct S s; s.a = 1; return f(s); }
''')
    check(command + [str(source), flag], 1, "f.uninitialised-read.1")
    source.write_text('''
struct S { int a; int b; };
struct S mk(void) { struct S s; s.a = 1; return s; }
int main(void) { struct S r = mk(); return r.a; }
''')
    check(command + [str(source), flag], 0, "VERIFICATION SUCCESSFUL")
    source.write_text('''
struct S { int a; int b; };
struct S mk(void) { struct S s; s.a = 1; return s; }
int main(void) { struct S r = mk(); return r.b; }
''')
    check(command + [str(source), flag], 1, "main.uninitialised-read.1")
    source.write_text('''
struct S { char c; int i; };
int main(void) { struct S a, b; a.c = 1; a.i = 2; b = a; return b.i; }
''')
    check(command + [str(source), flag], 0, "VERIFICATION SUCCESSFUL")
    source.write_text('''
struct S { char c; int i; };
int main(void) { struct S a, b; a.c = 1; b = a; return b.i; }
''')
    check(command + [str(source), flag], 1, "main.uninitialised-read.1")
    source.write_text('''
struct S { int a; int b; };
int f(struct S s, int n) { if (n) return f(s, n - 1); return s.a; }
int main(void) { struct S s; s.a = 1; return f(s, 3); }
''')
    check(command + [str(source), flag], 0, "VERIFICATION SUCCESSFUL")
    source.write_text('''
struct S { int a; int b; };
int f(struct S s, int n) { if (n) return f(s, n - 1); return s.b; }
int main(void) { struct S s; s.a = 1; return f(s, 3); }
''')
    check(command + [str(source), flag], 1, "f.uninitialised-read.1")

    # A scalar passed to a function that uses it is still read at the call.
    source.write_text('''
__attribute__((noinline)) int f(int v) { return v + 1; }
int main(void) { int x; return f(x); }
''')
    check(command + [str(source), flag], 1, "main.uninitialised-read.1")

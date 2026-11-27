// RUN: --unwind 6
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Unknown call receiving a conditional pointer built from exposed addresses:
// havocWith updates each candidate object once without quadratic state merges,
// and non-candidate objects keep their values.
#include <stdint.h>

extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
extern void ext_unknown(int *p);

int main(void) {
  int a = 1, b = 2, c = 3, untouched = 42;
  uintptr_t pa = (uintptr_t)&a;
  uintptr_t pb = (uintptr_t)&b;
  uintptr_t pc = (uintptr_t)&c;
  int sel = __VERIFIER_nondet_int();
  uintptr_t addr = sel == 0 ? pa : (sel == 1 ? pb : pc);
  int *p = (int *)addr;
  ext_unknown(p);
  __VERIFIER_assert(untouched == 42);
  if (sel == 0)
    __VERIFIER_assert(b == 2 && c == 3);
  return 0;
}

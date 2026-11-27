// RUN: --unwind 6
// EXPECT: ^VERIFICATION FAILED$
// The unknown call may still write the object a conditional pointer selects:
// with sel == 0, p is &a, so a need not keep its value (the companion of
// mem_havoc_ite_pointer, which checks the objects p cannot point to).
#include <stdint.h>

extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
extern void ext_unknown(int *p);

int main(void) {
  int a = 1, b = 2, c = 3;
  uintptr_t pa = (uintptr_t)&a;
  uintptr_t pb = (uintptr_t)&b;
  uintptr_t pc = (uintptr_t)&c;
  int sel = __VERIFIER_nondet_int();
  uintptr_t addr = sel == 0 ? pa : (sel == 1 ? pb : pc);
  int *p = (int *)addr;
  ext_unknown(p);
  if (sel == 0)
    __VERIFIER_assert(a == 1);
  return 0;
}

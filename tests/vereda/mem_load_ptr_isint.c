// Pointer loads with provenance isInt == false, isInt == true, and symbolic
// isInt all preserve object identity, dereference values, and null checks.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdint.h>

extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);

int main(void) {
  int x = 11, y = 22;

  // isInt == false: ordinary pointer stored and loaded back.
  int *slot1 = __VERIFIER_nondet_int() ? &x : (int *)0;
  int * volatile v1 = slot1;
  int *p1 = v1;
  __VERIFIER_assert((p1 == 0) == (slot1 == 0));
  if (p1 != 0)
    __VERIFIER_assert(*p1 == 11);

  // isInt == true: integer address stored into pointer slot and loaded back.
  uintptr_t addr = __VERIFIER_nondet_int() ? (uintptr_t)&y : (uintptr_t)0;
  int * volatile v2;
  *(uintptr_t *)&v2 = addr;
  int *p2 = v2;
  __VERIFIER_assert((p2 == 0) == (addr == 0));
  if (p2 != 0)
    __VERIFIER_assert(*p2 == 22);

  // Symbolic isInt: one branch stores a real pointer, the other an integer address.
  int c = __VERIFIER_nondet_int();
  int *slot3;
  if (c)
    slot3 = &x;
  else
    *(uintptr_t *)&slot3 = (uintptr_t)&y;
  int *p3 = slot3;
  __VERIFIER_assert(p3 != 0);
  __VERIFIER_assert(*p3 == (c ? 11 : 22));
  return 0;
}

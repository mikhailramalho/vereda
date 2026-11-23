// A pointer made from an integer before an object came to sit at that
// address should compare equal to a pointer to the object (C11 6.5.9p6 with
// 6.3.2.3p5). Read back from memory it does (mem_inttoptr_copy); kept in a
// register it still points nowhere, and pointer equality compares object and
// offset. Comparing addresses instead would let an undef pointer equal an
// object's address without pointing into it (esbmc/github_8517_assume_pins_pointer).
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: KNOWNBUG
#include <stdint.h>
#include <stdlib.h>
extern void __VERIFIER_assert(int);
extern unsigned long __VERIFIER_nondet_ulong(void);

int main(void) {
  uintptr_t a = __VERIFIER_nondet_ulong();
  int *p = (int *)a;
  int *q = malloc(sizeof *q);
  if (q && (uintptr_t)q == a)
    __VERIFIER_assert(p == q);
  return 0;
}

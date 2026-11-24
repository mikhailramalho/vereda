// A pointer made from an integer before an object came to sit at that
// address compares equal to a pointer to the object (C11 6.5.9p6 with
// 6.3.2.3p5). Read back from memory it is converted again
// (mem_inttoptr_copy); kept in a register it points outside the program, and
// a comparison with such a pointer compares addresses (#137).
// EXPECT: ^VERIFICATION SUCCESSFUL$
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

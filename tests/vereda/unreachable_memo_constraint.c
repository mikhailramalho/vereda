// Without --unwind, unreachable() memoises SAT guards by term id, and must
// drop cached SAT answers whenever a global constraint is added mid-walk.
// Here iteration 1 caches the loop's guard as SAT, then allocates a VLA whose
// size exceeds PTRDIFF_MAX, adding a global constraint that makes the exact
// same guard UNSAT on iteration 2. Without epoch invalidation the loop would
// keep unrolling past iteration 2.
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT-NOT: iteration 2
#include <stddef.h>
#include <stdint.h>

extern int __VERIFIER_nondet_int(void);
extern size_t __VERIFIER_nondet_size_t(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

int main(void) {
  int c = __VERIFIER_nondet_int();
  size_t n = __VERIFIER_nondet_size_t();
  __VERIFIER_assume(n > (size_t)PTRDIFF_MAX);
  int iter = 0;
  while (c) {
    if (iter == 1) {
      char vla[n];
      vla[0] = 0;
    }
    iter++;
  }
  __VERIFIER_assert(iter <= 2);
  return 0;
}

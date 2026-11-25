// An address made of an object's address plus an offset within it converts
// back to that object at that offset as the walk builds it (#224, ESBMC
// #9785): the layout keeps an object's end from wrapping, so the solver
// gets nothing to search, nor does the exposed object b laid out after a.
// The forms: the address itself, a constant added on either side, and one
// past the end. Pointer checks stay on, and fold too.
// RUN: --dump-smt-only
// EXPECT: ^\(assert false\)\n\(check-sat\)
#include <stdint.h>
extern void __VERIFIER_assert(int);
long a[4], b[4];
uintptr_t saved;
// Not inlined, so that the walker, not LLVM, adds the offset.
__attribute__((noinline)) long *at(uintptr_t x, uintptr_t k) { return (long *)(x + k); }
__attribute__((noinline)) long *from(uintptr_t k, uintptr_t x) { return (long *)(k + x); }
int main(void) {
  saved = (uintptr_t)b;
  uintptr_t x = (uintptr_t)a;
  *at(x, 0) = 1;
  *at(x, sizeof(long)) = 2;
  *from(3 * sizeof(long), x) = 4;
  __VERIFIER_assert(a[0] == 1 && a[1] == 2 && a[3] == 4 && at(x, sizeof a) == a + 4);
  return 0;
}

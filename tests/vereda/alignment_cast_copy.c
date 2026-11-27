// A struct read through a cast of a declared array's address at an odd
// offset is misaligned (ESBMC's github_10891). Clang copies it with a memcpy
// of align 1, the alignment it proves from the array, so the check takes the
// struct's alignment from the other side of the copy.
// RUN: --ub-alignment-check
// RUN: --ub-alignment-check -m32
// EXPECT: ^  dereference failure: Incorrect alignment when accessing data object$
// EXPECT: ^VERIFICATION FAILED$
#include <stdint.h>
unsigned __VERIFIER_nondet_uint(void);
void __VERIFIER_assume(int);
struct N { uint32_t v; };
int main(void) {
  uint32_t buf[2] = {1, 2};
  unsigned o = __VERIFIER_nondet_uint();
  __VERIFIER_assume(o <= 4);
  struct N r = *(struct N *)((unsigned char *)buf + o);
  return r.v;
}

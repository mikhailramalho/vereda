// A memcpy of a size with no bound into an object too large to write byte
// by byte is a lambda over the destination's bytes: those below the size
// come from the source, the others, known bytes included, stay.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);
char big[4096], other[4096];

int main(void) {
  unsigned n = __VERIFIER_nondet_uint();
  big[5] = 7;
  big[4000] = 9;
  other[0] = 3;
  other[4000] = 4;
  memcpy(big, other, n);
  __VERIFIER_assert(n <= 4000 || big[4000] == 4);
  __VERIFIER_assert(n > 4000 || big[4000] == 9);
  __VERIFIER_assert(n < 1 || big[0] == 3);
  __VERIFIER_assert(n > 5 || big[5] == 7);
  return 0;
}

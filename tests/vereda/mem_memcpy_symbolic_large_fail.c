// The known byte at 4000 is overwritten once the size passes it. A size
// past the object is #29's bounds check, not tested here.
// EXPECT: ^  FAILED       \[main\.assertion\.1\]  line 15  __VERIFIER_assert$
// EXPECT: ^VERIFICATION FAILED$
#include <string.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);
char big[4096], other[4096];

int main(void) {
  unsigned n = __VERIFIER_nondet_uint();
  big[4000] = 9;
  other[4000] = 4;
  memcpy(big, other, n);
  __VERIFIER_assert(big[4000] == 9);
  return 0;
}

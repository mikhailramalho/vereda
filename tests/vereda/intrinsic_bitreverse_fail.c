// Non-palindromic bit patterns differ from their bit reversal.
// EXPECT: ^VERIFICATION FAILED$
#include <stdint.h>

extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);

int main(void) {
  uint32_t x = __VERIFIER_nondet_uint();
  __VERIFIER_assert(__builtin_bitreverse32(x) == x);
  return 0;
}

#include <stdio.h>
int main() {
  unsigned char c = __VERIFIER_nondet_uchar();
  char b[8];
  sprintf(b, "%o", c);
  __VERIFIER_assert(b[1] != 0); // false: c<8 prints one digit
  return 0;
}
// Imported from ESBMC regression/esbmc/sprintf_symbolic_digits_fail
// EXPECT: ^VERIFICATION FAILED$

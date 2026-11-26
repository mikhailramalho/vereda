// #10786: C17 6.5.3.4p2 evaluates the size of a VLA operand of sizeof.
#include <assert.h>

unsigned char __VERIFIER_nondet_uchar(void);

int main()
{
  int n = 1;
  sizeof(int[0 * n++]);
  sizeof(int[3][0 * n++]);
  assert(n == 3);

  unsigned char m = __VERIFIER_nondet_uchar();
  __VERIFIER_assume(m > 0 && m < 10);
  assert(sizeof(int[m][3]) == 12u * m);
  assert(sizeof(char[3][m]) == 3u * m);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10786_side_effect
// RUN: --std c11
// EXPECT: ^VERIFICATION SUCCESSFUL$

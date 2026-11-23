/* #10087: a store at a dynamic offset that starts in a sub-byte bit-field. */
#include <assert.h>
struct S
{
  unsigned int a : 4;
  unsigned int b : 28;
  unsigned int c;
};
struct S s[2];
int main(void)
{
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 2);
  *(unsigned int *)&s[i] = 0x31u;
  assert(s[i].a == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10087_fail
// Needs: --little-endian
// RUN: --little-endian
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none

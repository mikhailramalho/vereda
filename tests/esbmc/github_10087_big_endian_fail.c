/* #10087: on big-endian targets the first bit-field holds the high bits. */
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
  *(unsigned int *)&s[i] = 0x10000031u;
  assert(s[i].a == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10087_big_endian_fail
// Needs: --big-endian
// RUN: --big-endian --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none

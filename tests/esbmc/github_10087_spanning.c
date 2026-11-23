/* #10087: a store starting in a whole-byte member and ending in bit-fields. */
#include <assert.h>
struct S { unsigned short h; unsigned short a : 4, b : 12; unsigned int c; };
struct S s[2];
int main(void)
{
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 2);
  *(unsigned int *)&s[i] = 0x12340005u;
  assert(s[i].h == 5);
  assert(s[i].a == 4);
  assert(s[i].b == 0x123);
  assert(*(unsigned int *)&s[i] == 0x12340005u);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10087_spanning
// Needs: --little-endian
// RUN: --little-endian
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

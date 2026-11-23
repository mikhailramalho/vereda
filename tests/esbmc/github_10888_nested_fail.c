/* #10888: the second member is not read from the first one. */
#include <assert.h>
struct U
{
  unsigned char f1, f2;
};
struct G
{
  struct U f1, f2;
};
struct T
{
  struct U a, b;
};
unsigned char __VERIFIER_nondet_uchar(void);
int main(void)
{
  struct T t = {
    {__VERIFIER_nondet_uchar(), __VERIFIER_nondet_uchar()}, {__VERIFIER_nondet_uchar(), __VERIFIER_nondet_uchar()}};
  struct G g = *(struct G *)&t;
  assert(g.f2.f1 == t.a.f1);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10888_nested_fail
// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$

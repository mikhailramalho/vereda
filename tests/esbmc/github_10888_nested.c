/* #10888: a struct of struct members read through another struct with the
 * same layout and different member names. */
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
  assert(g.f1.f1 == t.a.f1 && g.f1.f2 == t.a.f2);
  assert(g.f2.f1 == t.b.f1 && g.f2.f2 == t.b.f2);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10888_nested
// RUN: --multi-property
// EXPECT: ^VERIFICATION SUCCESSFUL$

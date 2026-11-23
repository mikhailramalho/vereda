/* #10888: the second array member is read from its own bytes. */
#include <assert.h>
struct G
{
  unsigned char f1[2], f2[2];
};
struct T
{
  unsigned char a[2], b[2];
};
unsigned char __VERIFIER_nondet_uchar(void);
int main(void)
{
  struct T t = {
    {__VERIFIER_nondet_uchar(), __VERIFIER_nondet_uchar()}, {__VERIFIER_nondet_uchar(), __VERIFIER_nondet_uchar()}};
  struct G g = *(struct G *)&t;
  assert(g.f1[0] == t.a[0] && g.f2[1] == t.b[1]);
  assert(g.f2[0] == t.a[0]);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10888_arrays_fail
// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$

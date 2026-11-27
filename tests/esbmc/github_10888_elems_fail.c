/* #10888: the second member comes from the next element. */
#include <assert.h>
struct U
{
  unsigned char f1, f2;
};
struct G
{
  struct U f1, f2;
};
unsigned char __VERIFIER_nondet_uchar(void);
int main(void)
{
  struct U arr[3] = {
    {__VERIFIER_nondet_uchar(), __VERIFIER_nondet_uchar()},
    {__VERIFIER_nondet_uchar(), __VERIFIER_nondet_uchar()},
    {__VERIFIER_nondet_uchar(), __VERIFIER_nondet_uchar()}};
  struct G g = *(struct G *)&arr[1];
  assert(g.f2.f1 == arr[1].f1);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10888_elems_fail
// RUN: --multi-property --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$

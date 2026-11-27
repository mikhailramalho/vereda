/* #10888: a struct spanning past the last element is still out of bounds. */
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
  struct U arr[2] = {
    {__VERIFIER_nondet_uchar(), __VERIFIER_nondet_uchar()}, {__VERIFIER_nondet_uchar(), __VERIFIER_nondet_uchar()}};
  struct G g = *(struct G *)&arr[1];
  return g.f1.f1;
}
// Imported from ESBMC regression/esbmc/github_10888_elems_oob_fail
// RUN: --multi-property --no-incremental-bmc
// EXPECT: ^  FAILED .* dereference failure: array bounds violated$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  FAILED .* dereference failure: array bounds violated$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL

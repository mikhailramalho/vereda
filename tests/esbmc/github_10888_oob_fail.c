/* #10888: a struct wider than its object is still out of bounds. */
struct G
{
  unsigned char f1[2], f2[2];
};
struct H
{
  unsigned char a[2];
};
unsigned char __VERIFIER_nondet_uchar(void);
int main(void)
{
  struct H h = {{__VERIFIER_nondet_uchar(), __VERIFIER_nondet_uchar()}};
  struct G g = *(struct G *)&h;
  return g.f2[0];
}
// Imported from ESBMC regression/esbmc/github_10888_oob_fail
// RUN: --multi-property
// EXPECT: ^  FAILED .* dereference failure: Access to object out of bounds$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  FAILED .* dereference failure: Access to object out of bounds$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL

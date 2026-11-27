/* Negative regression test: assertion must FAIL.
 * forall b: (b>0) || forall(a, a>0)
 * At b=0: b>0 is false; forall a: a>0 is false (a=0) => false.
 * So the outer forall is false, not 1 => assertion fails. */
int main()
{
  int a1, b1;

  __VERIFIER_assert(
    __ESBMC_forall(&b1, (b1 > 0) || __ESBMC_forall(&a1, a1 > 0)) == 1
); // forall b: (b>0)||forall(a,a>0) should be false — assertion must fail

  return 0;
}
// Imported from ESBMC regression/z3/github_3886_4_false
// Needs: __ESBMC_forall
// RUN: --solver z3 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$

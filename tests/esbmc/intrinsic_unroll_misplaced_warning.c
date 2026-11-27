/* Test that a misplaced __ESBMC_unroll(N) that is not directly followed by
 * a loop produces a warning instead of being silently ignored.
 */

int __VERIFIER_nondet_int();

int main()
{
  __ESBMC_unroll(5);
  int x = __VERIFIER_nondet_int();
  if(x > 0)
    x = 1;

  return x;
}
// Imported from ESBMC regression/esbmc/intrinsic_unroll_misplaced_warning
// Needs: __ESBMC_unroll
// MODE: FUTURE
// EXPECT: __ESBMC_unroll at .* is not directly followed by a loop
// EXPECT: ^VERIFICATION SUCCESSFUL$

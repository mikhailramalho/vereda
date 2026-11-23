/* Test __ESBMC_unroll(N) with a while loop.
 * Loop bound is 10, intrinsic sets unroll to 10.
 * Verification succeeds with no unwinding assertions.
 */

int main()
{
  int i = 0;

  #pragma unroll 11
  while(i < 10)
    i++;

  return 0;
}
// Imported from ESBMC regression/esbmc/intrinsic_unroll_while_true
// Needs: --verbosity 9
// RUN: --verbosity 9
// MODE: FUTURE
// EXPECT: Applying #pragma unroll 11 to loop
// EXPECT: ^VERIFICATION SUCCESSFUL$

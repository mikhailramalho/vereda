/* Test __ESBMC_unroll(N) with a for loop.
 * Loop bound is 5, intrinsic sets unroll to 5.
 * Verification succeeds with no unwinding assertions.
 */

int main()
{
  int sum = 0;

  #pragma unroll 6
  for(int i = 0; i < 5; i++)
    sum += i;

  return 0;
}
// Imported from ESBMC regression/esbmc/intrinsic_unroll_for_true
// RUN:
// MODE: FUTURE
// EXPECT: Applying #pragma unroll 6 to loop
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict SUCCESSFUL

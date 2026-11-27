/* Test __ESBMC_unroll(N) with N smaller than the iteration count of a
 * multi-variable for loop. The loop runs 5 times but the intrinsic only
 * unrolls 2 times, so an unwinding assertion must fail.
 */

int main()
{
  int sum = 0;

  #pragma unroll 3
  for(int i = 0, j = 10; i < j; i++, j--)
    sum += i;

  return 0;
}
// Imported from ESBMC regression/esbmc/intrinsic_unroll_for_multivar_fail
// RUN: --no-incremental-bmc
// MODE: FUTURE
// EXPECT: Applying #pragma unroll 3 to loop
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict SUCCESSFUL

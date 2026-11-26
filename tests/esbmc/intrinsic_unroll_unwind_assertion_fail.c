/* Test that __ESBMC_unroll(N) with N smaller than the loop bound
 * triggers an unwinding assertion failure.
 * Loop runs 10 times but intrinsic only unrolls 3 times.
 */

int main()
{
  int i = 0;

  #pragma unroll 4
  while(i < 10)
    i++;

  return 0;
}
// Imported from ESBMC regression/esbmc/intrinsic_unroll_unwind_assertion_fail
// RUN:
// MODE: FUTURE
// EXPECT: Applying #pragma unroll 4 to loop
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict SUCCESSFUL

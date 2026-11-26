/* Test __ESBMC_unroll(N) applied in multiple functions.
 * main() uses __ESBMC_unroll(5) before a for loop (bound 5).
 * foo() uses __ESBMC_unroll(10) before a while loop (bound 10).
 * Both loops should be independently annotated and verification succeeds.
 */

int foo();

int main()
{
  #pragma unroll 6
  for(int i = 0; i < 5; i++)
    ;

  foo();
  return 0;
}

int foo()
{
  int i = 0;
  #pragma unroll 11
  while(i < 10)
    i++;
  return i;
}
// Imported from ESBMC regression/esbmc/intrinsic_unroll_multi_function_true
// RUN:
// MODE: FUTURE
// EXPECT: Applying #pragma unroll 11 to loop
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict SUCCESSFUL

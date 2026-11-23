int nd(void);
int main()
{
  while (nd())
    ;
  char big[12]; /* 96 bits: over a 90-bit limit */
  big[0] = 0;
  return big[0];
}
// Imported from ESBMC regression/esbmc/loop_condition_stack_limit_fail
// Needs: --stack-limit 90
// RUN: --unwind 2 --no-unwinding-assertions --stack-limit 90
// MODE: FUTURE
// EXPECT: Stack limit property was violated when declaring big
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none

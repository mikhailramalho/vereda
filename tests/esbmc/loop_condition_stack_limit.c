int nd(void);
int main()
{
  while (nd())
    ;
  char big[12]; /* 96 bits: over a 90-bit limit */
  big[0] = 0;
  return big[0];
}
// Imported from ESBMC regression/esbmc/loop_condition_stack_limit
// Needs: --stack-limit 96
// RUN: --unwind 2 --no-unwinding-assertions --stack-limit 96
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

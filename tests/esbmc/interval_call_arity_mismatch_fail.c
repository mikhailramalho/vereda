/* The first call binds the parameter; the second passes nothing, so the
   binding must not carry over. */
void f();

int main(void)
{
  f(7);
  f();
  return 0;
}

void f(int a)
{
  __VERIFIER_assert(a == 7); // the second call passes no argument
}
// Imported from ESBMC regression/esbmc/interval_call_arity_mismatch_fail
// Needs: --std c17 --interval-analysis
// RUN: --std c17 --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none

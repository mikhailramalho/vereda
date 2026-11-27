int main()
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x > 0);
  __VERIFIER_assert(x > 1); // x exceeds one
  return 0;
}
// Imported from ESBMC regression/esbmc/segfault_handler_flag_fail
// Needs: --segfault-handler
// RUN: --segfault-handler
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none

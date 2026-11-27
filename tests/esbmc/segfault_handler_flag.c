int main()
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x > 0);
  __VERIFIER_assert(x > 0); // x stays positive
  return 0;
}
// Imported from ESBMC regression/esbmc/segfault_handler_flag
// Needs: --segfault-handler
// RUN: --segfault-handler --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

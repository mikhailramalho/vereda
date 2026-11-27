// --dead-code-check reports only on the base-case pass, so combining it with a
// standalone phase mode (--forward-condition here) would exit without ever
// printing the findings. The combination must be rejected up front.
int main(void)
{
  return 0;
}
// Imported from ESBMC regression/esbmc/cwe_dead_code_reject_phase
// Needs: --dead-code-check --forward-condition
// RUN: --dead-code-check --forward-condition
// MODE: FUTURE
// EXPECT: ^ERROR: --dead-code-check cannot be combined with --forward-condition$
// Vereda: verdict none

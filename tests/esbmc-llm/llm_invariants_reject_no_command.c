int main(void)
{
  return 0;
}
// Imported from ESBMC regression/llm/llm_invariants_reject_no_command
// Needs: --llm-invariants
// RUN: --llm-invariants
// MODE: FUTURE
// EXPECT: ^ERROR: --llm-invariants needs --llm-command$
// Vereda: verdict none

int main(void)
{
  return 0;
}
// Imported from ESBMC regression/llm/llm_invariants_reject_command_only
// Needs: --llm-command model.py
// RUN: --llm-command model.py
// MODE: FUTURE
// EXPECT: ^ERROR: --llm-command needs --llm-invariants$
// Vereda: verdict none

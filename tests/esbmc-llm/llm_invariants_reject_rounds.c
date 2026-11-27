int main(void)
{
  return 0;
}
// Imported from ESBMC regression/llm/llm_invariants_reject_rounds
// Needs: --llm-invariants --llm-command model.py --llm-max-rounds 0
// RUN: --llm-invariants --llm-command model.py --llm-max-rounds 0 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^ERROR: --llm-max-rounds must be at least 1$
// Vereda: verdict none

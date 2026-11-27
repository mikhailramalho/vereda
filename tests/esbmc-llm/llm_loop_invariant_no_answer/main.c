/* The model command fails every time: each round is skipped with a warning
 * and the run goes on without invariants, unwinding the bounded loop. */
#include <assert.h>

int main(void)
{
  unsigned i = 0, x = 0;
  while (i < 4)
  {
    x += 2;
    i++;
  }
  assert(x == 8);
  return 0;
}
// Imported from ESBMC regression/llm/llm_loop_invariant_no_answer
// Needs: --llm-invariants --llm-command model.sh --llm-max-rounds 2 REQUIRES posix_host
// RUN: --llm-invariants --llm-command model.sh --llm-max-rounds 2 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^WARNING: LLM loop invariants: no answer: CLI LLM process exited with 1$
// EXPECT: ^LLM loop invariants: round 2, asking the model$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

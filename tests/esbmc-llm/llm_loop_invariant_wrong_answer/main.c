/* As llm_loop_invariant_cegis_fail, but the model answers with an invariant
 * that would mask the false assertion and does not hold on entry. The check
 * drops it, so the bug is never reported as SUCCESSFUL. */
#include <assert.h>

unsigned __VERIFIER_nondet_uint(void);

int main(void)
{
  unsigned n = __VERIFIER_nondet_uint();
  unsigned i = 0, x = 0, y = 0;

  while (i < n)
  {
    if (i % 2 == 0)
      x++;
    else
      y++;
    i++;
  }

  assert(x + y == n + 1);
  return 0;
}
// Imported from ESBMC regression/llm/llm_loop_invariant_wrong_answer
// Needs: --llm-invariants --llm-command model.py --llm-max-rounds 1 REQUIRES posix_host
// RUN: --llm-invariants --llm-command model.py --llm-max-rounds 1 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^LLM loop invariants: 1 of 2 candidate conjunct\(s\) are inductive$
// EXPECT: \A(?![\s\S]*VERIFICATION SUCCESSFUL)[\s\S]*\Z
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none

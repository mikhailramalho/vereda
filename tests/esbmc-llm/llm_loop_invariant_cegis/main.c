/* The branch in the body puts this loop out of reach of the affine
 * synthesiser. The scripted model first answers with one wrong and one weak
 * invariant; the feedback leads it to add the counter bound. */
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

  assert(x + y == n);
  return 0;
}
// Imported from ESBMC regression/llm/llm_loop_invariant_cegis
// Needs: --llm-invariants --llm-command model.py REQUIRES posix_host
// RUN: --llm-invariants --llm-command model.py
// MODE: FUTURE
// EXPECT: ^LLM loop invariants: 1 of 2 candidate conjunct\(s\) are inductive$
// EXPECT: ^LLM loop invariants: round 2, asking the model$
// EXPECT: ^Synthesised loop invariants for 1 loop$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

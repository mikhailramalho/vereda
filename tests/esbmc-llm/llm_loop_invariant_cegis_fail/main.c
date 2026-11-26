/* As llm_loop_invariant_cegis, with a genuinely false assertion: the model's
 * invariants are inductive and refute it, so it is reported FAILED rather than
 * masked or left unknown. */
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
// Imported from ESBMC regression/llm/llm_loop_invariant_cegis_fail
// Needs: --llm-invariants --llm-command model.py REQUIRES posix_host
// RUN: --llm-invariants --llm-command model.py
// MODE: FUTURE
// EXPECT: ^LLM loop invariants: 2 of 2 candidate conjunct\(s\) are inductive$
// EXPECT: ^Synthesised loop invariants for 1 loop$
// EXPECT: \A(?![\s\S]*round 2)[\s\S]*\Z
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none

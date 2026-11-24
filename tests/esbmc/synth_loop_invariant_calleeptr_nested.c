/* The marker's own call is direct, but the callee reaches an indirect one, so
 * closing over the call graph still runs out of names. The whole-program stand
 * down covers that case too, not only an indirect call in the marker block. */
static unsigned int count_to(unsigned int n)
{
  unsigned int i = 0;
  unsigned int s = 0;
  while (i < n)
  {
    s = s + 1;
    i = i + 1;
  }
  return s;
}

static unsigned int (*fp)(unsigned int) = count_to;

static unsigned int outer(unsigned int n)
{
  return fp(n);
}

int main(void)
{
  unsigned int i = 0;

  __ESBMC_loop_invariant(i <= 4);
  __ESBMC_loop_invariant(outer(4) == 4);
  while (i < 4)
    i++;

  __VERIFIER_assert(i == 4); // end
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_calleeptr_nested
// Needs: __ESBMC_loop_invariant
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants --unwind 6
// MODE: FUTURE
// EXPECT: \A(?![\s\S]*Synthesised loop invariants)[\s\S]*\Z
// EXPECT: ^WARNING: --synthesise-loop-invariants: a user-written loop invariant reaches a call through a function pointer, whose callee cannot be named\. Synthesising nothing rather than risk weakening that invariant$
// EXPECT: ^VERIFICATION SUCCESSFUL$

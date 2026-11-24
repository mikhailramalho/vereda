/* The user's invariant calls through a function pointer, so the callee whose
 * loops must be left alone cannot be named. Synthesis stands down for the whole
 * program rather than guess which function the marker depends on. */
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

int main(void)
{
  unsigned int i = 0;

  __ESBMC_loop_invariant(i <= 4);
  __ESBMC_loop_invariant(fp(4) == 4);
  while (i < 4)
    i++;

  __VERIFIER_assert(i == 4); // end
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_calleeptr
// Needs: __ESBMC_loop_invariant
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants --unwind 6
// MODE: FUTURE
// EXPECT: \A(?![\s\S]*Synthesised loop invariants)[\s\S]*\Z
// EXPECT: ^WARNING: --synthesise-loop-invariants: a user-written loop invariant reaches a call through a function pointer, whose callee cannot be named\. Synthesising nothing rather than risk weakening that invariant$
// EXPECT: ^VERIFICATION SUCCESSFUL$

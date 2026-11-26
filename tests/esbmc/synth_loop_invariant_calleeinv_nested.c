/* The loop that must be left alone sits two calls below the marker, so reaching
 * it is the call-graph closure's worklist and nothing else: protecting only the
 * callees named in the marker's own block stops at outer(), and count_to's loop
 * is then cut. The marker would read a havoc-abstracted return -- the user's
 * invariant evaluated on a value the loop never produced. */
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

static unsigned int mid(unsigned int n)
{
  return count_to(n);
}

static unsigned int outer(unsigned int n)
{
  return mid(n);
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
// Imported from ESBMC regression/esbmc/synth_loop_invariant_calleeinv_nested
// Needs: __ESBMC_loop_invariant
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants --unwind 6 --multi-property
// MODE: FUTURE
// EXPECT: \A(?![\s\S]*Synthesised loop invariants)[\s\S]*\Z
// EXPECT: ^WARNING: --synthesise-loop-invariants added no invariant: no loop matched the affine counter/accumulator shape, or every match was declined; the run proceeds unchanged$
// EXPECT: ^  PASSED       \[main\.assertion\.\d+\]  line 34  loop invariant base case \[i <= 4\]$
// EXPECT: ^  PASSED       \[main\.assertion\.\d+\]  line 34  loop invariant base case \[return_value\S* == 4\]$
// EXPECT: ^VERIFICATION SUCCESSFUL$

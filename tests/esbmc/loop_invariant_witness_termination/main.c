/* The witness path reaches goto_loop_invariant_combined too, so the same
 * exemption applies: a witness-injected loop invariant does not put an ASSERT
 * in the loop head's slot, and --termination's havoc composes with it. */
unsigned int __VERIFIER_nondet_uint(void);

int main(void)
{
  unsigned int i = 0;
  unsigned int n = __VERIFIER_nondet_uint();
  unsigned int s = 0;

  while (i != n)
  {
    s = s + 1;
    i = i + 1;
  }

  return 0;
}
// Imported from ESBMC regression/esbmc/loop_invariant_witness_termination
// Needs: --validate-correctness-witness --witness witness.yml --termination
// RUN: --validate-correctness-witness --witness witness.yml --termination
// MODE: FUTURE
// EXPECT: ^Injecting loop invariant at line 12: s == i$
// EXPECT: \A(?![\s\S]*cannot be combined with --termination)[\s\S]*\Z
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none

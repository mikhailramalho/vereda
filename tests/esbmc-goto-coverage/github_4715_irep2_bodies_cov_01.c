#include <assert.h>

// Assertion coverage under --irep2-bodies (esbmc#4715). The two asserts back
// onto a side_effect_exprt("function_call") for __VERIFIER_assert; sideeffect2t
// carries no source location, so the IREP2 body round-trip used to strip the
// ASSERT instruction's location. --assertion-coverage gates its instrumentation
// on the source filename, so a location-less assert was silently dropped and
// the run reported "Total Asserts: 0" / SUCCESSFUL. convert_expression now
// restores the statement location onto the side effect, so both asserts are
// counted and reached (FAILED, because assertion coverage negates them).
void test()
{
  int x = 1;
  for (int i = 0; i < 2; i++)
    assert(x == 1);
}

int main()
{
  test();
}
// Imported from ESBMC regression/goto-coverage/github_4715_irep2_bodies_cov_01
// Needs: --assertion-coverage --irep2-bodies
// RUN: --assertion-coverage --unwind 3 --no-unwinding-assertions --irep2-bodies
// MODE: FUTURE
// EXPECT: ^Total Asserts: 1$
// EXPECT: ^Total Assertion Instances: 2$
// EXPECT: ^Reached Assertion Instances: 2$
// EXPECT: ^Assertion Instances Coverage: 100%$
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none

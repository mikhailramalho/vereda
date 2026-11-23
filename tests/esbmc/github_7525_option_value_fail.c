/* The failing pair for #7525. `--no-assertions` is swallowed as
 * --smtlib-solver-prog's value, so assertions stay on and this reports FAILED.
 * Honoured, it would report SUCCESSFUL -- the verdict is what pins the
 * swallowing, not just the warning text. */
#include <assert.h>
int main(void)
{
  int x = 0;
  assert(x == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7525_option_value_fail
// Needs: --smtlib-solver-prog --no-assertions
// RUN: --smtlib-solver-prog --no-assertions --unwind 1
// MODE: FUTURE
// EXPECT: ^WARNING: .--smtlib-solver-prog. takes a value, so .--no-assertions. was read as that value
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none

/* #7525: the parser runs with boost's allow_guessing, so an abbreviated
 * option is still an option. `--show-loo` reaches --show-loops and is
 * swallowed as --smtlib-solver-prog's value; an exact-match check would let
 * this spelling through unwarned. */
int main(void)
{
  int x = 0;
  return x;
}
// Imported from ESBMC regression/esbmc/github_7525_option_value_abbrev
// Needs: --smtlib-solver-prog --show-loo
// RUN: --smtlib-solver-prog --show-loo --unwind 1
// MODE: FUTURE
// EXPECT: ^WARNING: .--smtlib-solver-prog. takes a value, so .--show-loo. was read as that value
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

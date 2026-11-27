/* github #7896: the negative twin of github_7896_nondet -- the solver must
 * reach a verdict on a binary16 sort, not abort on an unsupported format.
 * A FAILED verdict needs a model satisfying the assume, so this is also what
 * keeps the twin non-vacuous on a backend that accepts the old (4, 12) sort
 * instead of rejecting it. */
_Float16 __VERIFIER_nondet_h(void);

int main(void)
{
  _Float16 x = __VERIFIER_nondet_h();
  __VERIFIER_assume(x == (_Float16)1000.0f);
  __VERIFIER_assert(x + x == (_Float16)2001.0f); // 2000 != 2001
  return 0;
}
// Imported from ESBMC regression/floats/github_7896_nondet_fail
// EXPECT: ^VERIFICATION FAILED$

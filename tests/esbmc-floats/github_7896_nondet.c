/* github #7896: a _Float16 that reaches the solver. The 4-bit exponent made
 * the sort (4, 12), which Bitwuzla rejects as an experimental format. */
_Float16 __VERIFIER_nondet_h(void);

int main(void)
{
  _Float16 x = __VERIFIER_nondet_h();
  __VERIFIER_assume(x == (_Float16)1000.0f);
  __VERIFIER_assert(x + x == (_Float16)2000.0f); // 2000 is exact in binary16
  return 0;
}
// Imported from ESBMC regression/floats/github_7896_nondet
// EXPECT: ^VERIFICATION SUCCESSFUL$

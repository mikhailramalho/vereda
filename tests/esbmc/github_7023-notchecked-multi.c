int main()
{
  int a, b;

  __VERIFIER_assert(a + b == b + a); // addition commutes
  /* At most one of these can be violated by any single model, so exactly one
     stays undecided whichever one the solver picks. */
  __VERIFIER_assert(a != 1); // a is not one
  __VERIFIER_assert(a != 2); // a is not two
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7023-notchecked-multi
// RUN: --multi-property --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \A(?![\s\S]*NOT CHECKED)[\s\S]*\Z

// __builtin_assume(cond) must constrain exactly cond, no more: assuming x > 5
// does not imply x > 10, so this assertion must fail. Guards against an
// over-strong (unsound) assume model. See #4606.
int main()
{
  int x;
  __builtin_assume(x > 5);
  __VERIFIER_assert(x > 10); // x > 5 does not imply x > 10
  return 0;
}
// Imported from ESBMC regression/esbmc/builtin_assume_unsound_fail
// EXPECT: ^VERIFICATION FAILED$

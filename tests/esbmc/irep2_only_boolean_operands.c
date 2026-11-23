int foo(int x) { return 1; }
int main(void)
{
  int il;
  for (il = 0; foo(il) && il < 2; ++il)
  {
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_boolean_operands
// Needs: --goto-functions-only --clang-c-irep2-adjust-only
// RUN: --goto-functions-only --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: IF !\(\(_Bool\)return_value\$_foo\$1 \?
// Vereda: verdict none

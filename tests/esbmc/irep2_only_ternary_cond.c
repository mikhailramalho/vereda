int f(void);
int main(void)
{
  int x = 1;
  int y = x ? f() : 2;
  return y;
}
// Imported from ESBMC regression/esbmc/irep2_only_ternary_cond
// Needs: --goto-functions-only --clang-c-irep2-adjust-only
// RUN: --goto-functions-only --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: IF !\(_Bool\)x THEN GOTO
// Vereda: verdict none

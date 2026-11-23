int g(int x)
{
  return x + 1;
}

int main(void)
{
  /* The cast is a no-op once the function designator decays. The default path
     emits `p=&g`; the IREP2 adjust pass keeps the cast
     (scope-clang-c-irep2.md 143). */
  int (*p)(int) = (int (*)(int))g;
  return p(1) == 2 ? 0 : 1;
}
// Imported from ESBMC regression/esbmc/github_4715_fnptr_cast_collapse_irep2
// Needs: --clang-c-irep2-adjust-only --goto-functions-only
// RUN: --clang-c-irep2-adjust-only --goto-functions-only
// MODE: FUTURE
// EXPECT: ^ +ASSIGN p=&g;$
// Vereda: verdict none

/* A separate assignment statement, not an initialiser: clang inserts the decay
   for `int *p = a;` itself, but leaves `p = a;` to the adjuster. */
int main(void)
{
  int a[3];
  int *p;
  a[0] = 7;
  p = a;
  return p[0];
}
// Imported from ESBMC regression/esbmc/irep2_only_assign_array_decay
// Needs: --goto-functions-only --clang-c-irep2-adjust-only
// RUN: --goto-functions-only --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: ASSIGN p=&a\[0\]
// Vereda: verdict none

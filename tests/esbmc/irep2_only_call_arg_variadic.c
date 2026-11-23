/* A variadic argument has no parameter type to convert against, so only the
   array decay is owed -- to void *. */
int vfun(int n, ...);

int main(void)
{
  int a[3];
  a[0] = 1;
  return vfun(1, a);
}
// Imported from ESBMC regression/esbmc/irep2_only_call_arg_variadic
// Needs: --goto-functions-only --clang-c-irep2-adjust-only
// RUN: --goto-functions-only --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: vfun\(1, \(void \*\)\(&a\[0\]\)\)
// Vereda: verdict none

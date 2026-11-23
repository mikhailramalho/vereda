/* The lifted helper has internal linkage, so its symbol id carries the basename
   of the file the transform wrote. That name must not vary between runs. */
int main(void)
{
  int base = 40;
  int add(int v)
  {
    return base + v;
  }
  __VERIFIER_assert(add(2) == 42); // the lifted nested function is called
  return 0;
}
// Imported from ESBMC regression/esbmc/gcc_nested_func_stable_name
// Needs: --gcc-nested-functions --no-library --goto-functions-only
// RUN: --gcc-nested-functions --no-library --goto-functions-only
// MODE: FUTURE
// EXPECT: esbmc-nested\.gcc_nested_func_stable_name\.c@F@__esbmc_nested_
// Vereda: verdict none

// Regression test for #4098: __label__ must not be misclassified as a
// typedef variable and captured by nested functions.
int main(void)
{
  __label__ done;
  int x = 0;
  void inc(void)
  {
    x++;
  }
  inc();
  inc();
  __VERIFIER_assert(x == 2); // __label__ not captured
  goto done;
  x = 99;
done:
  return 0;
}
// Imported from ESBMC regression/esbmc/gcc_nested_func_label
// Needs: --gcc-nested-functions --no-library
// RUN: --gcc-nested-functions --no-library
// MODE: FUTURE
// EXPECT: GCC __label__ declarations are not supported
// Vereda: verdict none

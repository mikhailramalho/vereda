// Basic nested function with captured write
int main()
{
  int x = 0;
  void inc()
  {
    x++;
  }
  inc();
  __VERIFIER_assert(x == 1); // nested function modified enclosing var
  return 0;
}
// Imported from ESBMC regression/esbmc/gcc_nested_func_01
// Needs: --gcc-nested-functions --no-library
// RUN: --gcc-nested-functions --no-library --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

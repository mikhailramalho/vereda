// Nested function reads enclosing variable
int main()
{
  int val = 7;
  int get()
  {
    return val;
  }
  int r = get();
  __VERIFIER_assert(r == 7); // nested func reads enclosing var
  return 0;
}
// Imported from ESBMC regression/esbmc/gcc_nested_func_02
// Needs: --gcc-nested-functions --no-library
// RUN: --gcc-nested-functions --no-library --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

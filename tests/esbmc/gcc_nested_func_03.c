// Nested function with own parameters and capture
int main()
{
  int base = 10;
  int add(int y)
  {
    return base + y;
  }
  int r = add(5);
  __VERIFIER_assert(r == 15); // nested func with params and capture
  return 0;
}
// Imported from ESBMC regression/esbmc/gcc_nested_func_03
// Needs: --gcc-nested-functions --no-library
// RUN: --gcc-nested-functions --no-library
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

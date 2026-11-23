// Two levels of nesting
int main()
{
  int x = 0;
  void outer()
  {
    void inner()
    {
      x = 99;
    }
    inner();
  }
  outer();
  __VERIFIER_assert(x == 99); // doubly nested function
  return 0;
}
// Imported from ESBMC regression/esbmc/gcc_nested_func_05
// Needs: --gcc-nested-functions --no-library
// RUN: --gcc-nested-functions --no-library
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

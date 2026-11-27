// Failing assertion: verifier should catch the bug
int main()
{
  int x = 0;
  void inc()
  {
    x++;
  }
  inc();
  __VERIFIER_assert(x == 99); // should fail: x is 1 not 99
  return 0;
}
// Imported from ESBMC regression/esbmc/gcc_nested_func_06
// Needs: --gcc-nested-functions --no-library
// RUN: --gcc-nested-functions --no-library
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none

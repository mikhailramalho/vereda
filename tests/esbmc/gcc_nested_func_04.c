// Nested function passed as function pointer
void call_it(void (*f)(void))
{
  f();
}

int main()
{
  int x = 0;
  void set()
  {
    x = 42;
  }
  call_it(set);
  __VERIFIER_assert(x == 42); // nested func called via pointer
  return 0;
}
// Imported from ESBMC regression/esbmc/gcc_nested_func_04
// Needs: --gcc-nested-functions --no-library
// RUN: --gcc-nested-functions --no-library --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

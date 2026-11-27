// Regression: capture param name must not collide with user's __capture_x.
int main(void)
{
  int x = 10;
  int result(void)
  {
    int __capture_x = 42;
    x += __capture_x;
    return x;
  }
  int r = result();
  __VERIFIER_assert(r == 52); // capture collision
  __VERIFIER_assert(x == 52); // captured x updated
  return 0;
}
// Imported from ESBMC regression/esbmc/gcc_nested_func_capture_collision
// Needs: --gcc-nested-functions --no-library
// RUN: --gcc-nested-functions --no-library --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

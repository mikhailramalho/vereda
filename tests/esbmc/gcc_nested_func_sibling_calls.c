// Nested `a` calls sibling `b`; `b` captures `x` and `y`.  Transitive
// capture propagation must extend `a`'s signature with pointer params
// for both x and y so that `a` can forward them to `b` in the lifted
// call.  Order of extra args must match `x` then `y` (enclosing-locals
// order).
int main()
{
  int x = 0, y = 0;
  void b(void)
  {
    x = 42;
    y = 43;
  }
  void a(void) { b(); }
  a();
  __VERIFIER_assert(x == 42); // sibling call forwards capture x
  __VERIFIER_assert(y == 43); // sibling call forwards capture y
  return 0;
}
// Imported from ESBMC regression/esbmc/gcc_nested_func_sibling_calls
// Needs: --gcc-nested-functions --no-library
// RUN: --gcc-nested-functions --no-library
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

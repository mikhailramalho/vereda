// Nested `a` calls sibling `c` which captures nothing.  The call to
// `c()` must NOT receive any injected arguments — spurious args would
// cause a signature mismatch and compile failure under Clang's real
// parse pass.
int main()
{
  int seen = 0;
  int c(void) { return 1; }
  void a(void) { seen = c(); }
  a();
  __VERIFIER_assert(seen == 1); // uncaptured sibling: no stray args
  return 0;
}
// Imported from ESBMC regression/esbmc/gcc_nested_func_sibling_calls_uncaptured
// Needs: --gcc-nested-functions --no-library
// RUN: --gcc-nested-functions --no-library --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

typedef int func();

/* No definition, and no address-taken function of this signature anywhere in
 * the program: the call through f has no candidate target. */
func *fun();

int main()
{
  func *f = fun();
  int x = f ? f() : 0;
  __VERIFIER_assert(!x); // x is 0 unless an unknown callee supplied a value
  return 0;
}
// Imported from ESBMC regression/esbmc/github_604
// Needs: --closed-world-fnptr
// RUN: --closed-world-fnptr --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

double __VERIFIER_nondet_double();

int main()
{
  double x = __VERIFIER_nondet_double();
  assert(x==x);
  return 0;
}
// Imported from ESBMC regression/floats/nan_double
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$

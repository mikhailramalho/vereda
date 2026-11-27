int main()
{
  float a, b;
  __VERIFIER_assert((a>b)==(a-b>0)); // theorem
}
// Imported from ESBMC regression/floats/Float23
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$

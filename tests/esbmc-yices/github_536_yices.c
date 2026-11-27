float __VERIFIER_nondet_float();

int main()
{
  float x = __VERIFIER_nondet_float();
  assert(x==x);
  return 0;
}
// Imported from ESBMC regression/yices/github_536_yices
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$

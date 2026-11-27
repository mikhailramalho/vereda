float __VERIFIER_nondet_float();

int main()
{
  float x = __VERIFIER_nondet_float();
  assert(x == x);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_536
// EXPECT: ^VERIFICATION FAILED$

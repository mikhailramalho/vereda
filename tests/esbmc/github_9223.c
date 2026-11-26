int __VERIFIER_nondet_int(void);
int main(void)
{
  int a = __VERIFIER_nondet_int();
  __VERIFIER_assume(a >= 0 && a < 1024);
  int b = __VERIFIER_nondet_int();
  return a << (b & 15);
}
// Imported from ESBMC regression/esbmc/github_9223
// RUN: --overflow-check --solver z3 --std c11
// EXPECT: ^VERIFICATION SUCCESSFUL$

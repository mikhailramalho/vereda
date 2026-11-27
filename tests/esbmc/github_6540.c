int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assert(x != 42); // may fail
  __VERIFIER_assert(x >= 0 || x < 0); // holds
  return 0;
}
// Imported from ESBMC regression/esbmc/github_6540
// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$

int g;

int bump(void)
{
  return ++g;
}

int h(void)
{
  int a = bump();
  return bump() * 10 + a;
}

int main()
{
  int x;
  __VERIFIER_assert(
    __ESBMC_forall(&x, h() == 12)); // reordered evaluation would prove this
}
// Imported from ESBMC regression/z3/github_6100_seq_fail
// Needs: __ESBMC_forall
// RUN: --solver z3 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$

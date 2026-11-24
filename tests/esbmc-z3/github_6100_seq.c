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
  __VERIFIER_assert(__ESBMC_forall(&x, h() == 21)); // h returns 21 on first call
}
// Imported from ESBMC regression/z3/github_6100_seq
// Needs: __ESBMC_forall
// RUN: --solver z3
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$

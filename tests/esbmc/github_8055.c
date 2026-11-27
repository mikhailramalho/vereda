// #8055
int __VERIFIER_nondet_int();

int f(void)
{
  int r = __VERIFIER_nondet_int();
  return r;
}

int main()
{
  int n = 0, i = 0;
  n = f();
  while (i < n)
  {
    i++;
    if (i > 3)
      break;
  }
  __VERIFIER_assert(i == 0); // loop not entered
}
// Imported from ESBMC regression/esbmc/github_8055
// EXPECT: ^VERIFICATION FAILED$

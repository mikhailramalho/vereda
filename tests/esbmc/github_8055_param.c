// #8055
int __VERIFIER_nondet_int();

void f(int n)
{
  int i = 0;
  while (i < n)
  {
    i++;
    if (i > 3)
      break;
  }
  __VERIFIER_assert(i == 0); // loop not entered
  n = 0;
}

int main()
{
  f(0);
  f(__VERIFIER_nondet_int());
}
// Imported from ESBMC regression/esbmc/github_8055_param
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$

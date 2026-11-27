// #8055
int __VERIFIER_nondet_int();

void f(int n, int d)
{
  if (d > 0)
  {
    f(0, d - 1);
    int i = 0;
    while (i < n)
    {
      i++;
      if (i > 3)
        break;
    }
    __VERIFIER_assert(i == 0); // loop not entered
  }
  n = 0;
}

int main()
{
  f(__VERIFIER_nondet_int(), 1);
}
// Imported from ESBMC regression/esbmc/github_8055_recursion
// EXPECT: ^VERIFICATION FAILED$

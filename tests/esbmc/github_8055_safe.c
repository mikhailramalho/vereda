// #8055
int __VERIFIER_nondet_int();

int f(void)
{
  int r = __VERIFIER_nondet_int();
  __VERIFIER_assume(r <= 0);
  return r;
}

void g(int n)
{
  int i = 0;
  while (i < n)
  {
    i++;
    if (i > 3)
      break;
  }
  __VERIFIER_assert(i == 0); // loop not entered
  n = 5;
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
  g(0);
  g(-__VERIFIER_nondet_int() * 0);
}
// Imported from ESBMC regression/esbmc/github_8055_safe
// EXPECT: ^VERIFICATION SUCCESSFUL$

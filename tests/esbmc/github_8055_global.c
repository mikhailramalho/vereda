// #8055
int __VERIFIER_nondet_int();
int g;

int f(void)
{
  g = 0;
  return __VERIFIER_nondet_int();
}

int main()
{
  int i = 0;
  g = f();
  while (i < g)
  {
    i++;
    if (i > 3)
      break;
  }
  __VERIFIER_assert(i == 0); // loop not entered
}
// Imported from ESBMC regression/esbmc/github_8055_global
// EXPECT: ^VERIFICATION FAILED$

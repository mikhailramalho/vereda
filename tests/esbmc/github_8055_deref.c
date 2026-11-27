// #8055
int ext(void);

int main()
{
  int n = 0, i = 0;
  int *p = &n;
  *p = ext();
  while (i < n)
  {
    i++;
    if (i > 3)
      break;
  }
  __VERIFIER_assert(i == 0); // loop not entered
}
// Imported from ESBMC regression/esbmc/github_8055_deref
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$

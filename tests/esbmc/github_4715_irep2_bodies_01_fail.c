// Failing variant: same structured-CF under --irep2-bodies; one assertion
// is deliberately wrong so the verdict must be VERIFICATION FAILED.

int main()
{
  int x = 0;

  if (1)
    x = 1;

  while (x < 3)
    x++;

  for (int i = 0; i < 2; i++)
    x += i;

  int z = 0;
  switch (x)
  {
  case 5:
    z = 99;
    break;
  default:
    z = 0;
    break;
  }

  // x started at 1, while brings it to 3, for adds 0+1=1 → x==4
  // wrong assertion: should be x==4 not x==5
  __VERIFIER_assert(x == 5); // wrong: x should be 4

  return 0;
}
// Imported from ESBMC regression/esbmc/github_4715_irep2_bodies_01_fail
// Needs: --irep2-bodies --bv
// RUN: --irep2-bodies --bv
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none

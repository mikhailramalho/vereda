int main()
{
  int x[1][1][3];
  x[0][0][0] = __VERIFIER_nondet_int();
  x[0][0][1] = __VERIFIER_nondet_int();
  x[0][0][2] = __VERIFIER_nondet_int();
  __VERIFIER_assume((x[0][0][0] >= 77) && (x[0][0][0] <= 87));
  __VERIFIER_assume((x[0][0][1] >= -64) && (x[0][0][1] <= 64));
  __VERIFIER_assume((x[0][0][2] >= -64) && (x[0][0][2] <= 64));
  __VERIFIER_assert(0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_661_2_false
// EXPECT: ^VERIFICATION FAILED$

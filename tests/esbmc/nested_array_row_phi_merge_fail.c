int __VERIFIER_nondet_int(void);
int main(void)
{
  int a[2][2];
  int (*p)[2] = a;
  int i = __VERIFIER_nondet_int();
  int c = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 2);

  a[1][0] = 1;
  a[1][1] = 1;
  if (c)
    a[1][0] = 5;
  else
    a[1][1] = 7;

  __VERIFIER_assert(p[1][i] == 1 || p[1][i] == 5
); // the else arm's store survives the merge
  return 0;
}
// Imported from ESBMC regression/esbmc/nested_array_row_phi_merge_fail
// EXPECT: ^\s+the else arm.s store survives the merge$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^\s+the else arm.s store survives the merge$
// MODE: FUTURE
// Vereda: verdict FAILED

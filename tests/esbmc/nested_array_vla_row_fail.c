int __VERIFIER_nondet_int(void);

int main(void)
{
  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(n > 1 && n < 4);
  int a[n][n];
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 2);

  a[1][0] = 5;
  a[1][1] = 4;

  __VERIFIER_assert(a[1][i] == 4); // the symbolic-extent row's earlier store must not be dropped
  return 0;
}
// Imported from ESBMC regression/esbmc/nested_array_vla_row_fail
// EXPECT: ^\s+the symbolic-extent row.s earlier store must not be dropped$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^\s+the symbolic-extent row.s earlier store must not be dropped$
// MODE: FUTURE
// Vereda: verdict FAILED

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

  /* Coverage, not a gate: pre-PR master proves this too. The merge is the only
     shape here that hands decompose_stores() a two-armed chain, which the
     element-wise gates never build. */
  __VERIFIER_assert(p[1][i] == 1 || p[1][i] == 5 || p[1][i] == 7
); // a phi-merged array keeps every arm's stores
  return 0;
}
// Imported from ESBMC regression/esbmc/nested_array_row_phi_merge
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$

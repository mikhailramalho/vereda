int __VERIFIER_nondet_int(void);

int main(void)
{
  int a[2][2];
  int(*p)[2] = a;
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 2);

  a[1][0] = 5;
  a[1][1] = 4;

  __VERIFIER_assert(p[1][i] == 4); // the row's first store is still readable
  return 0;
}
// Imported from ESBMC regression/esbmc/nested_array_row_via_pointer_fail
// EXPECT: ^\s+the row.s first store is still readable$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^\s+the row.s first store is still readable$
// MODE: FUTURE
// Vereda: verdict FAILED

int __VERIFIER_nondet_int(void);

int main(void)
{
  int a[2][2][2];
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 2);

  a[0][0][0] = 1; a[0][0][1] = 2; a[0][1][0] = 3; a[0][1][1] = 4;
  a[1][0][0] = 5; a[1][0][1] = 6; a[1][1][0] = 7; a[1][1][1] = 8;

  int (*p)[2] = a[1];

  __VERIFIER_assert(p[1][i] > 6); // a plane read through a row pointer
  return 0;
}
// Imported from ESBMC regression/esbmc/nested_array_3d_row_pointer
// EXPECT: ^VERIFICATION SUCCESSFUL$

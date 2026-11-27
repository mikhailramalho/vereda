int __VERIFIER_nondet_int(void);
long __VERIFIER_nondet_long(void);
int main(void)
{
  long u = __VERIFIER_nondet_long();
  int m = __VERIFIER_nondet_int();
  __VERIFIER_assume(u == 4294967297L && m == 1);
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i <= 1);
  int b[2][u][m];
  b[0][1][0] = 1;
  b[i][0][0] = 2;
  __VERIFIER_assert(i == 0 || b[0][1][0] == 2); // b[1][0][0] is not b[0][1][0]
  return 0;
}
// Imported from ESBMC regression/esbmc/vla_long_size_truncation_fail
// EXPECT: ^VERIFICATION FAILED$

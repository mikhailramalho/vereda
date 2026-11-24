// The copy of summary_rows.c leaves out the last row, which the checks
// still compare.
// EXPECT: ^Summarised 3 array loop\(s\)$
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int n = 4, m = 300;
  int a[n][m], b[n][m];
  for (int i = 0; i < n; i++)
    for (int j = 0; j < m; j++)
      b[i][j] = __VERIFIER_nondet_int();
  for (int i = 0; i < n - 1; i++)
    for (int j = 0; j < m; j++)
      a[i][j] = b[i][j];
  for (int i = 0; i < n; i++)
    for (int j = 0; j < m; j++)
      __VERIFIER_assert(a[i][j] == b[i][j]);
  return 0;
}

// The transpose of summary_columns.c writes each column one row short.
// EXPECT: ^\[INFO\] Summarised 3 array loop\(s\)$
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int n = 4, m = 300;
  int a[n][m], c[m][n];
  for (int i = 0; i < n; i++)
    for (int j = 0; j < m; j++)
      a[i][j] = __VERIFIER_nondet_int();
  for (int i = 0; i < n; i++)
    for (int j = 0; j < m - 1; j++)
      c[j][i] = a[i][j];
  for (int i = 0; i < n; i++)
    for (int j = 0; j < m; j++)
      __VERIFIER_assert(c[j][i] == a[i][j]);
  return 0;
}

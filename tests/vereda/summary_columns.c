// array-multidimensional/transpose-u at 4 rows of 300: the inner loop of
// the transpose writes a column, elements a row apart. The checks read a
// column at a nondet row, an offset as far from every other column's
// elements, so they reach only that column's summary.
// EXPECT: ^Summarised 3 array loop\(s\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int n = 4, m = 300;
  int a[n][m], c[m][n];
  for (int i = 0; i < n; i++)
    for (int j = 0; j < m; j++)
      a[i][j] = __VERIFIER_nondet_int();
  for (int i = 0; i < n; i++)
    for (int j = 0; j < m; j++)
      c[j][i] = a[i][j];
  for (int i = 0; i < n; i++)
    for (int j = 0; j < m; j++)
      __VERIFIER_assert(c[j][i] == a[i][j]);
  return 0;
}

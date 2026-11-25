// array-multidimensional/copy-2-u at 4 rows of 300: each inner loop writes
// one row of a 2-D array, from an address SCEV gives as an AddRec of the
// outer loop, and is summarised in each outer iteration. The checks read a
// row at the nondet index of their own summary; a read reaches only that
// row's summary, and the copy's element folds to the one it was copied from.
// EXPECT: ^\[INFO\] Summarised 3 array loop\(s\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int n = 4, m = 300;
  int a[n][m], b[n][m];
  for (int i = 0; i < n; i++)
    for (int j = 0; j < m; j++)
      b[i][j] = __VERIFIER_nondet_int();
  for (int i = 0; i < n; i++)
    for (int j = 0; j < m; j++)
      a[i][j] = b[i][j];
  for (int i = 0; i < n; i++)
    for (int j = 0; j < m; j++)
      __VERIFIER_assert(a[i][j] == b[i][j]);
  return 0;
}

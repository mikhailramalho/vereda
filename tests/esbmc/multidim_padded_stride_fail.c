// Inner dimensions that are not powers of two, read and written at symbolic
// indices: constant initializer, whole-row store, and a union member.
unsigned __VERIFIER_nondet_uint(void);

int c[3][5][3] = {
  {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}, {10, 11, 12}, {13, 14, 15}},
  {{16, 17, 18}, {19, 20, 21}, {22, 23, 24}, {25, 26, 27}, {28, 29, 30}},
  {{31, 32, 33}, {34, 35, 36}, {37, 38, 39}, {40, 41, 42}, {43, 44, 45}}};

struct row
{
  int v[3];
};

union u
{
  int m[2][3];
  long long raw[3];
};

int main(void)
{
  unsigned i = __VERIFIER_nondet_uint(), j = __VERIFIER_nondet_uint(), k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 3 && j < 5 && k < 3);
  __VERIFIER_assert(c[i][j][k] == 15 * i + 3 * j + k + 1); // initializer

  int a[3][5];
  for (unsigned r = 0; r < 3; r++)
    for (unsigned s = 0; s < 5; s++)
      a[r][s] = 10 * r + s;
  a[i][j] = -1;
  __VERIFIER_assert(a[i][j] == -1); // store
  unsigned r = __VERIFIER_nondet_uint(), s = __VERIFIER_nondet_uint();
  __VERIFIER_assume(r < 3 && s < 5 && (r != i || s != j));
  __VERIFIER_assert(a[r][s] == 10 * r + s); // other cells
  __VERIFIER_assert(c[i][j][k] != 23); // reachable cell

  struct row rows[2][3] = {0};
  struct row x = {{7, 8, 9}};
  rows[1][2] = x;
  __VERIFIER_assert(rows[1][2].v[k] == 7 + k); // row

  union u w = {.m = {{1, 2, 3}, {4, 5, 6}}};
  unsigned p = __VERIFIER_nondet_uint();
  __VERIFIER_assume(p < 2);
  __VERIFIER_assert(w.m[p][k] == 3 * p + k + 1); // union
  return 0;
}
// Imported from ESBMC regression/esbmc/multidim_padded_stride_fail
// RUN: --unwind 6
// EXPECT: ^VERIFICATION FAILED$

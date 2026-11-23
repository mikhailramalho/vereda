// Arrays indexed at a symbolic offset whose address does not escape get
// arrays of their own, indexed by offset (#71). Each case below crosses the
// boundary of that: a pointer that may be any of three such arrays, which
// reaches memory as an unknown object; pointer comparisons into one; a join
// where both sides wrote one; a variable-length array created on one side of
// a branch only; and an array whose address escapes after a symbolic store.
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned __VERIFIER_nondet_uint(void);
extern int __VERIFIER_nondet_int(void);
int *g;

int main(void) {
  unsigned i = __VERIFIER_nondet_uint(), j = __VERIFIER_nondet_uint();
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 4 && j < 4 && k < 3);
  int a[4], b[4], c[4];
  a[j] = 1;
  b[j] = 2;
  c[j] = 3;

  // One of three arrays.
  int *p = k == 0 ? a : (k == 1 ? b : c);
  p[i] = 9;
  __VERIFIER_assert(p[i] == 9);
  if (k == 0)
    __VERIFIER_assert(a[i] == 9);
  if (k == 2 && i != j)
    __VERIFIER_assert(c[j] == 3 && b[j] == 2);

  // Comparisons of pointers into one array.
  int *x = &a[i], *y = &a[j];
  __VERIFIER_assert((x == y) == (i == j));
  __VERIFIER_assert((x < y) == (i < j));
  __VERIFIER_assert(x != (int *)b);

  // A join where each side wrote the array.
  if (__VERIFIER_nondet_int())
    b[i] = 5;
  else
    b[i] = 6;
  __VERIFIER_assert(b[i] == 5 || b[i] == 6);

  // An array that exists on one side of the branch only.
  if (__VERIFIER_nondet_int()) {
    int v[j + 1];
    v[i % (j + 1)] = 3;
    v[j] = 4;
    __VERIFIER_assert(v[j] == 4);
    __VERIFIER_assert(i % (j + 1) == j || v[i % (j + 1)] == 3);
  }

  // An array whose address escapes after a store at a symbolic offset.
  int e[4];
  e[j] = 7;
  g = e;
  __VERIFIER_assert(g[j] == 7);
  g[i] = 8;
  __VERIFIER_assert(e[i] == 8);
  return 0;
}

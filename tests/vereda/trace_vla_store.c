// A store to an element of a variable-length array shows in the
// counterexample with its indices, as one to a fixed-length array does.
// The outermost dimension's length takes part in no index; an inner one's
// is the value the trace last gave its length variable (__vla_expr2, m,
// 3 or 4, not n = 2), as ESBMC prints z[1][1] (#311).
// EXPECT: ^  v\[1\] = 7;
// EXPECT: ^  w\[1\]\[2\] = 8;
// EXPECT: ^  z\[1\]\[1\] = 9;
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

int main(void) {
  int k = __VERIFIER_nondet_int();
  int n = __VERIFIER_nondet_int();
  int m = __VERIFIER_nondet_int();
  if (n != 2 || m < 3 || m > 4)
    return 0;
  int v[n];
  int w[n][3];
  int z[2][m];
  v[1] = k + 2;
  w[1][2] = k + 3;
  z[1][1] = k + 4;
  __VERIFIER_assert(v[1] + w[1][2] + z[1][1] != 7 + 8 + 9);
  return 0;
}

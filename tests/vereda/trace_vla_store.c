// A store to an element of a variable-length array shows in the
// counterexample with its indices, as one to a fixed-length array does,
// when only the outermost dimension's length varies; with an inner one of
// variable length the store still shows nothing, as its index would need
// that length (#311).
// EXPECT: ^  v\[1\] = 7;
// EXPECT: ^  w\[1\]\[2\] = 8;
// EXPECT-NOT: ^  z\[
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

int main(void) {
  int k = __VERIFIER_nondet_int();
  int n = __VERIFIER_nondet_int();
  if (n < 2 || n > 3)
    return 0;
  int v[n];
  int w[n][3];
  int z[2][n];
  v[1] = k + 2;
  w[1][2] = k + 3;
  z[1][1] = k + 4;
  __VERIFIER_assert(v[1] + w[1][2] + z[1][1] != 7 + 8 + 9);
  return 0;
}

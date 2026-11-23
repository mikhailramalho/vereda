// RUN: -m32
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT-NOT: length_error
// A VLA whose sizes are constants becomes one fixed array after instcombine:
// here alloca [2700000000 x i32], 10.8 GB, more than a 32-bit address space
// holds. Such an object is a stack overflow, which the verifier assumes
// away, so the path ends at the declaration. Building its size as a 32-bit
// term used to abort with std::length_error (#59).
extern void __VERIFIER_assert(int);
extern unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  int m = 1000, n = 1500, p = 1800;
  unsigned a[m][n][p];
  unsigned i = __VERIFIER_nondet_uint() % 4, j = __VERIFIER_nondet_uint() % 4;
  a[0][0][i] = 1;
  __VERIFIER_assert(a[0][0][j] == 1); // would fail if the array existed
  return 0;
}

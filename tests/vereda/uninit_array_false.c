// The bytes of an uninitialised array or variable-length array are unknown,
// not zero: each assertion below can fail.
// RUN: --multi-property
// EXPECT: FAILED +\[main.assertion.1\]
// EXPECT: FAILED +\[main.assertion.2\]
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int a[4];
  __VERIFIER_assert(a[1] == 0);
  int n = __VERIFIER_nondet_int();
  if (n < 2 || n > 8)
    return 0;
  int v[n];
  __VERIFIER_assert(v[n - 1] == 0);
  return 0;
}

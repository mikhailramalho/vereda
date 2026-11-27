// C23's empty initialiser of a VLA is a memset of its nondet size.
// RUN: --std c23
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

int main(void) {
  unsigned n = __VERIFIER_nondet_uint(), i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n >= 1 && n <= 100 && i < n);
  int a[n] = {};
  __VERIFIER_assert(a[i] == 0);
  a[0] = 5;
  __VERIFIER_assert(i == 0 || a[i] == 0);
  return 0;
}

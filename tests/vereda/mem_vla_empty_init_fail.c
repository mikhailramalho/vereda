// After a store, a read at a nondet index of the zeroed VLA may see it.
// RUN: --std c23
// EXPECT: ^  FAILED       \[main\.assertion\.2\]  line 14  __VERIFIER_assert$
// EXPECT: ^VERIFICATION FAILED$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

int main(void) {
  unsigned n = __VERIFIER_nondet_uint(), i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n >= 1 && n <= 100 && i < n);
  int a[n] = {};
  a[n - 1] = 5;
  __VERIFIER_assert(a[i] == 0);
  return 0;
}

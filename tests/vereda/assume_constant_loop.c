// RUN: --unwind 11
// EXPECT: ^VERIFICATION SUCCESSFUL$
// With n == 10 assumed, i < n folds on each iteration and the loop ends at
// the 11th test, within the bound; each iteration's x is its own (#120).
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned n = __VERIFIER_nondet_uint(), i = 0, sum = 0;
  __VERIFIER_assume(n == 10);
  while (i < n) {
    unsigned x = __VERIFIER_nondet_uint();
    __VERIFIER_assume(x == i);
    sum += x;
    i++;
  }
  __VERIFIER_assert(i == 10 && sum == 45);
  return 0;
}

// RUN: --multi-property
// EXPECT: ^\[INFO\] Summarised 2 array loop\(s\)$
// EXPECT: ^\[INFO\] Walk time: .*\(3 checks,
// EXPECT: ^  SUCCESS +\[main\.assertion\.1\] +line 25
// EXPECT: ^  FAILED +\[main\.assertion\.2\] +line 27
// EXPECT: ^  FAILED +\[main\.assertion\.3\] +line 30
// A summarised loop's iteration at index k has k < n, so with n <= 90 the
// test of k > 90 folds and its reach_error goes; the loop may run fewer
// times than 90, so a[50] may stay 0 after it, and with m <= 100 the
// iteration reaches k = 91 (#120).
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
extern void reach_error(void);
unsigned char a[100], b[100];
int main(void) {
  unsigned n = __VERIFIER_nondet_uint(), m = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n <= 90);
  for (unsigned i = 0; i < n; i++) {
    if (i > 90)
      reach_error();
    a[i] = 1;
  }
  if (n > 60)
    __VERIFIER_assert(a[55] == 1);
  else
    __VERIFIER_assert(a[50] == 1);
  __VERIFIER_assume(m <= 100);
  for (unsigned i = 0; i < m; i++)
    __VERIFIER_assert(i <= 90);
  return 0;
}

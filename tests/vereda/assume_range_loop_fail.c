// RUN: --unwind 30 --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  FAILED +\[main\.assertion\.1\] +line 19
// EXPECT: ^  FAILED +\[main\.assertion\.2\] +line 25
// EXPECT: ^  FAILED +\[main\.assertion\.3\] +line 32
// EXPECT: ^  FAILED +\[main\.assertion\.4\] +line 38
// EXPECT-NOT: unwinding assertion
// assume_range_loop with each loop's last iteration needed for the
// failure: a range that cut one iteration too many would lose it (#120).
extern int __VERIFIER_nondet_int(void);
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  int n = __VERIFIER_nondet_int(), s = 0;
  __VERIFIER_assume(n <= 10);
  for (int i = 0; i < n; i++)
    s += __VERIFIER_nondet_int() & 1;
  __VERIFIER_assert(s < 10);

  unsigned m = __VERIFIER_nondet_uint();
  __VERIFIER_assume(m < 5);
  for (unsigned i = 0; i < m; i++)
    s += __VERIFIER_nondet_int() & 1;
  __VERIFIER_assert(s < 14);

  int k = __VERIFIER_nondet_int();
  if (k > 7)
    return 0;
  for (int i = 0; i < k; i += 2)
    s += __VERIFIER_nondet_int() & 1;
  __VERIFIER_assert(s < 18);

  int p = __VERIFIER_nondet_int();
  __VERIFIER_assume(p >= 0 && p < 4);
  for (long i = 0; i < p + 3; i++)
    s += __VERIFIER_nondet_int() & 1;
  __VERIFIER_assert(s < 24);
  return 0;
}

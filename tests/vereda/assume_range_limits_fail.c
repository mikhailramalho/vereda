// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT-NOT: SUCCESS +\[main\.assertion
// EXPECT: ^  FAILED +\[main\.assertion\.9\]
// assume_range_limits with each check reachable at a type's limit, where
// a range that wraps or crosses the sign bit must say nothing (#120): u at
// the greatest unsigned, u + 0x20 wrapping past it, signed values at the
// greatest and least, a signed bound read as unsigned (n <= 5 has negative
// n, the greatest unsigned values), and n - 1 for unsigned n >= 1.
extern int __VERIFIER_nondet_int(void);
extern unsigned __VERIFIER_nondet_uint(void);
extern long long __VERIFIER_nondet_longlong(void);
extern unsigned long long __VERIFIER_nondet_ulonglong(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned u = __VERIFIER_nondet_uint();
  __VERIFIER_assume(u >= 0xFFFFFFF0u);
  __VERIFIER_assert(u != 0xFFFFFFFFu);
  __VERIFIER_assert(u + 0x20 >= 0x20);
  unsigned long long x = __VERIFIER_nondet_ulonglong();
  __VERIFIER_assume(x > 0xFFFFFFFFFFFFFFF0ull);
  __VERIFIER_assert(x < 0xFFFFFFFFFFFFFFFFull);

  int s = __VERIFIER_nondet_int();
  __VERIFIER_assume(s >= 2147483645);
  __VERIFIER_assert(s < 2147483647);
  int t = __VERIFIER_nondet_int();
  __VERIFIER_assume(t <= -2147483646);
  __VERIFIER_assert(t > -2147483647 - 1);
  long long w = __VERIFIER_nondet_longlong();
  __VERIFIER_assume(w <= -9223372036854775807LL);
  __VERIFIER_assert(w != -9223372036854775807LL - 1);

  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(n <= 5);
  __VERIFIER_assert((unsigned)n <= 5u);
  __VERIFIER_assert((unsigned)n < 0xFFFFFFFFu);
  unsigned m = __VERIFIER_nondet_uint();
  __VERIFIER_assume(m >= 1);
  __VERIFIER_assert(m - 1 < 0xFFFFFFFEu);
  return 0;
}

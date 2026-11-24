// A bound that --k-step-from adds gets --k-step-from-time-limit: proving
// that bound 2 has no bug means proving 2^61 - 1 prime, far past 1 s, so the
// run passes over it and bound 3 finds the bug at once (#174).
// RUN: --incremental-bmc --property-file %S/properties/unreach-call.prp --k-step 2 --k-step-from 3 --k-step-from-time-limit 1
// EXPECT: ^Bound 2 passed over: no answer in 1 s$
// EXPECT: Checking bound k = 3$
// EXPECT: ^FALSE_REACH$
extern void reach_error(void);
extern int __VERIFIER_nondet_int(void);
extern unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  unsigned long long x = __VERIFIER_nondet_uint(), y = __VERIFIER_nondet_uint();
  int n = 0;
  while (__VERIFIER_nondet_int())
    n++;
  if (n == 2 || (n == 1 && x > 1 && y > 1 && x * y == 2305843009213693951ULL))
    reach_error();
  return 0;
}

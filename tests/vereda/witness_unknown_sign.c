// A nondet function whose sign depends on a typedef (pthread_t) gets no
// function_return waypoint: -1 and 0xffffffffffffffff differ for a
// narrower type, and the name does not tell which.
// RUN: --witness -
// EXPECT: entry_type: "violation_sequence"
// EXPECT: value: "\\\\result == 7"
// EXPECT-NOT: result == -1
// EXPECT-NOT: line: 16\n
// EXPECT: VERIFICATION FAILED
extern void __assert_fail(const char *, const char *, unsigned, const char *);
void reach_error(void) { __assert_fail("0", "witness_unknown_sign.c", 3, "reach_error"); }
extern unsigned long __VERIFIER_nondet_pthread_t(void);
extern unsigned int __VERIFIER_nondet_uint(void);
int main(void) {
  unsigned int n = __VERIFIER_nondet_uint();
  unsigned long t = __VERIFIER_nondet_pthread_t();
  if (n == 7 && t == (unsigned long)-1)
    reach_error();
  return 0;
}

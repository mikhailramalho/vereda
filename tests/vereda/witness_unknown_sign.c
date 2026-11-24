// A nondet function whose sign depends on a typedef (pthread_t) gets no
// function_return waypoint for a value whose top bit is set: -1 and
// 0xffffffffffffffff differ for a narrower type, and the name does not tell
// which. A value whose top bit is clear reads the same in either sign, and
// a narrow result carries its sign in the ABI's zeroext.
// RUN: --witness -
// EXPECT: entry_type: "violation_sequence"
// EXPECT: value: "\\\\result == 7"
// EXPECT-NOT: result == -1
// EXPECT-NOT: line: 22\n
// EXPECT: value: "\\\\result == 12"(\n.*){3}\n +line: 23\n
// EXPECT: value: "\\\\result == 200"(\n.*){3}\n +line: 24\n
// EXPECT: VERIFICATION FAILED
extern void __assert_fail(const char *, const char *, unsigned, const char *);
void reach_error(void) { __assert_fail("0", "witness_unknown_sign.c", 3, "reach_error"); }
extern unsigned long __VERIFIER_nondet_pthread_t(void);
extern unsigned long __VERIFIER_nondet_sector_t(void);
extern unsigned char __VERIFIER_nondet_uint8_t(void);
extern unsigned int __VERIFIER_nondet_uint(void);
int main(void) {
  unsigned int n = __VERIFIER_nondet_uint();
  unsigned long t = __VERIFIER_nondet_pthread_t();
  unsigned long s = __VERIFIER_nondet_sector_t();
  unsigned char b = __VERIFIER_nondet_uint8_t();
  if (n == 7 && t == (unsigned long)-1 && s == 12 && b == 200)
    reach_error();
  return 0;
}

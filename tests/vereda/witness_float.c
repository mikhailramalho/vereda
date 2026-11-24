// A floating value gets a hexadecimal constant, which is exact where a
// decimal one is rounded; an infinity is beyond the largest finite value;
// a NaN, which no constant equals, gets no waypoint.
// RUN: --witness -
// EXPECT: value: "\\\\result == 0x1\.8p0"(\n.*){3}\n +line: 16\n
// EXPECT: value: "\\\\result == -0x1\.999999999999ap-4"(\n.*){3}\n +line: 17\n
// EXPECT: value: "\\\\result < -0x1\.fffffffffffffp1023"(\n.*){3}\n +line: 18\n
// EXPECT-NOT: line: 19\n
// EXPECT: VERIFICATION FAILED
extern void __assert_fail(const char *, const char *, unsigned, const char *);
void reach_error(void) { __assert_fail("0", "witness_float.c", 3, "reach_error"); }
extern float __VERIFIER_nondet_float(void);
extern double __VERIFIER_nondet_double(void);
int main(void) {
  // Each call has a line of its own so that its waypoint shows by its line.
  float f = __VERIFIER_nondet_float();
  double d = __VERIFIER_nondet_double();
  double inf = __VERIFIER_nondet_double();
  double nan = __VERIFIER_nondet_double();
  if (f == 1.5f && d == -0.1 && inf == -__builtin_inf() && nan != nan)
    reach_error();
  return 0;
}

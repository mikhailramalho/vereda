// A validator binds each function_return waypoint to the next call at its
// location, so a reached call at a site that gets waypoints must get one
// too. INT64_MIN has no decimal constant and used to get none, and the
// waypoint of the second call below then bound to the first (#76).
// RUN: --witness -
// EXPECT: entry_type: "violation_sequence"
// EXPECT: value: "\\\\result == -9223372036854775807 - 1"[\s\S]*value: "\\\\result == 5"
// EXPECT: VERIFICATION FAILED
extern void __assert_fail(const char *, const char *, unsigned, const char *);
void reach_error(void) { __assert_fail("0", "witness_int64_min.c", 3, "reach_error"); }
extern long __VERIFIER_nondet_long(void);
int main(void) {
  long v[2];
  for (int i = 0; i < 2; i++)
    v[i] = __VERIFIER_nondet_long();
  if (v[0] == -9223372036854775807L - 1 && v[1] == 5)
    reach_error();
  return 0;
}

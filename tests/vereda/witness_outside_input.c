// No witness when the call to reach_error is outside the input files, here
// through #line, as a header or a .i file's linemarkers would put it: a
// validator rejects a waypoint in a file the task does not list.
// RUN: --witness -
// EXPECT: no witness: the call to reach_error is in /\S*/elsewhere\.h, not in an input file
// EXPECT-NOT: entry_type
// EXPECT: VERIFICATION FAILED
extern void __assert_fail(const char *, const char *, unsigned, const char *);
void reach_error(void) { __assert_fail("0", "witness_outside_input.c", 3, "reach_error"); }
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x == 3)
#line 1 "elsewhere.h"
    reach_error();
  return 0;
}

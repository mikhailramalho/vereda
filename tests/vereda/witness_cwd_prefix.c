// Run from the test's own directory, clang splits the location's file into
// that directory and the bare name, while the compile unit keeps the path it
// was given (#85). Joined, both are the input file, so the witness names its
// target in it.
// CWD: %S
// RUN: --witness -
// EXPECT: entry_type: "violation_sequence"
// EXPECT: type: "target"\n.*\n +location:\n +file_name: "/\S*/witness_cwd_prefix\.c"\n +line: 17\n
// EXPECT-NOT: no witness
// EXPECT: VERIFICATION FAILED
extern void __assert_fail(const char *, const char *, unsigned, const char *);
void reach_error(void) { __assert_fail("0", "witness_cwd_prefix.c", 3, "reach_error"); }
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x == 7)
    reach_error();
  return 0;
}

// The resumed trace keeps the prefix's nondet call and reaches the later target.
// RUN: --no-summarize-array-loops --incremental-bmc --witness -
// EXPECT: \[PROGRESS\] Resuming main
// EXPECT: entry_type: "violation_sequence"
// EXPECT: value: "\\\\result == 7"
// EXPECT: type: "target"
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __assert_fail(const char *, const char *, unsigned, const char *);
void reach_error(void) { __assert_fail("0", "incremental_resume_witness.c", 10, "reach_error"); }
int main(void) {
  int token = __VERIFIER_nondet_int();
  for (int i = 0; i < 8; ++i)
    if (i == 5 && token == 7)
      reach_error();
}

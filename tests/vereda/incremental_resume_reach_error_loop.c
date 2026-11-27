// A checkpoint inside reach_error retains the outer call used as witness target.
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 8 --witness -
// EXPECT: \[PROGRESS\] Resuming main
// EXPECT: FAILED\s+\[reach_error\.assertion\.1\]
// EXPECT: entry_type: "violation_sequence"
// EXPECT: value: "\\\\result == 7"
// EXPECT: type: "target"
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __assert_fail(const char *, const char *, unsigned, const char *);

void reach_error(void) {
  for (int i = 0; i < 6; ++i)
    __VERIFIER_nondet_int();
  __assert_fail("0", __FILE__, __LINE__, "reach_error");
}

int main(void) {
  int token = __VERIFIER_nondet_int();
  if (token == 7)
    reach_error();
}

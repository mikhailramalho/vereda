// A bug newly reachable below a recursive cut retains its witness and claim id.
// RUN: --incremental-bmc --max-k-step 10 --witness -
// EXPECT: \[PROGRESS\] Resuming main
// EXPECT: FAILED\s+\[reach_error\.assertion\.1\]
// EXPECT: entry_type: "violation_sequence"
// EXPECT: value: "\\\\result == 7"
// EXPECT: type: "target"
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __assert_fail(const char *, const char *, unsigned, const char *);
void reach_error(void) { __assert_fail("0", __FILE__, __LINE__, "reach_error"); }

static void descend(int depth, int token) {
  if (depth == 0) {
    if (token == 7)
      reach_error();
    return;
  }
  descend(depth - 1, token);
}

int main(void) {
  int token = __VERIFIER_nondet_int();
  descend(6, token);
}

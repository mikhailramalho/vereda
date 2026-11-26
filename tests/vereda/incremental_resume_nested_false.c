// A bug first reachable in a later outer iteration keeps its property id and
// witness across checkpoints at different nesting depths.
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 8 --witness -
// EXPECT: \[PROGRESS\] Resuming main
// EXPECT: FAILED\s+\[reach_error\.assertion\.1\]
// EXPECT: entry_type: "violation_sequence"
// EXPECT: value: "\\\\result == 7"
// EXPECT: type: "target"
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void __assert_fail(const char *, const char *, unsigned, const char *);
void reach_error(void) { __assert_fail("0", __FILE__, __LINE__, "reach_error"); }

int main(void) {
  int token = __VERIFIER_nondet_int();
  __VERIFIER_assume(token >= 0 && token < 8);
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 6; ++j)
      if (i == 2 && j == 4 && token == 7)
        reach_error();
}

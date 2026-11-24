// A nondet bound: the sum reaches 10 only when n == 5, inside the bound.
// The assertion sits in the loop body, so every iteration ORs into one claim.
// RUN: --unwind 6
// EXPECT: ^  FAILED       \[main\.assertion\.1\]  line 18  __VERIFIER_assert$
// EXPECT: ^\*\* 1 of 1 properties failed$
// EXPECT: ^// State \d+ file \S*loop_nondet_false\.c line 15 function main thread 0\n  n = 5;
// EXPECT: ^  s = 8; // 00000000 00000000 00000000 00001000\n\n// State \d+ .*\n  i = 4; .*\n\n// State \d+ file \S*loop_nondet_false\.c line 18 column 5 function main thread 0\n/\*\nViolated property:$
// EXPECT-NOT: s = 10
// EXPECT: ^VERIFICATION FAILED$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

int main(void) {
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n <= 5);
  for (unsigned i = 0, s = 0; i < n; i++) {
    __VERIFIER_assert(s != 8);
    s += 2;
  }
  return 0;
}

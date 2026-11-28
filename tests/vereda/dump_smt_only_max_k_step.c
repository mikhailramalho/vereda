// Without --unlimited-k-steps, --dump-smt-only on a loop that never
// finishes stops at --max-k-step too, and says so.
// RUN: --dump-smt-only --max-k-step 3
// EXPECT: ^; bound 3$
// EXPECT: ^\[INFO\] Stopped at bound 3 \(--max-k-step 3\) with loops still open$
// EXPECT-NOT: ^; bound 4$
// EXPECT-NOT: VERIFICATION
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int s = 0;
  while (__VERIFIER_nondet_int())
    s++;
  __VERIFIER_assert(s != 7);
  return 0;
}

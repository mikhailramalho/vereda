// --dump-smt-only solves nothing, so under --unlimited-k-steps no verdict
// would end the run on a loop that never finishes: it stops at the largest
// bound, --max-k-step (100 by default), and says so.
// RUN: --dump-smt-only --unlimited-k-steps
// EXPECT: ^; bound 100$
// EXPECT: ^\[INFO\] Stopped at bound 100 \(--max-k-step 100\) with loops still open$
// EXPECT-NOT: ^; bound 101$
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

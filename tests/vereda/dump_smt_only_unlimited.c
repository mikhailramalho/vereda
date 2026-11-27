// --dump-smt-only solves nothing, so under --unlimited-k-steps no verdict
// would end the run on a loop that never finishes: it stops at the largest
// bound, --max-k-step (384 by default), and says so.
// RUN: --dump-smt-only --unlimited-k-steps
// EXPECT: ^; bound 384$
// EXPECT: ^\[INFO\] Stopped at bound 384 \(--max-k-step 384\) with loops still open$
// EXPECT-NOT: ^; bound 385$
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

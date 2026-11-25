// --incremental-bmc stops at the first bound whose unwinding assertions
// hold: the loop runs four times, so bound 5 unwinds it fully.
// RUN: --incremental-bmc
// EXPECT: ^\[INFO\] Bound 4 too small: unwinding assertion loop 0$
// EXPECT-NOT: ^\[INFO\] Bound 5 too small
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);

int main(void) {
  int s = 0;
  for (int i = 0; i < 4; ++i)
    s += i;
  __VERIFIER_assert(s == 6);
  return 0;
}

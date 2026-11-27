// Under incremental BMC, --dump-smt-only prints each bound's formula after a
// line naming the bound, without solving, and goes on until the walk finishes
// the loop: bound 7 unwinds its six iterations. Bounds 3, 5 and 6 only add
// iterations of a loop SCEV bounds, so they are skipped as when solving.
// RUN: --dump-smt-only --no-summarize-array-loops
// EXPECT: ^; bound 1\n\(set-logic
// EXPECT: ^; bound 2$
// EXPECT: ^; bound 4$
// EXPECT: ^; bound 7$
// EXPECT-NOT: ^; bound (3|5|6|8)$
// EXPECT-NOT: VERIFICATION
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int s = 0, n = __VERIFIER_nondet_int();
  for (int i = 0; i < 6; i++)
    s += n;
  __VERIFIER_assert(s != 7);
  return 0;
}

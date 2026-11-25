// Small constant-trip-count loops skip unreachable() up to their SCEV
// backedge-taken count even under a symbolic (or UNSAT) guard, and still
// terminate via unreachable() past that count when folding does not settle
// the exit condition.
// RUN: --no-summarize-array-loops
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern int __VERIFIER_nondet_int(void);
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);

int main(void) {
  int a = __VERIFIER_nondet_int();
  int b = __VERIFIER_nondet_int();
  int s = 0;
  // Infeasible symbolic guard (not folded to false): skipping unreachable()
  // on iterations 1..3 must not cause a spurious failure.
  if (a > b && b > a) {
    for (int i = 0; i < 4; ++i)
      s += 10;
    __VERIFIER_assert(0);
  }
  // Symbolic start with constant SCEV backedge count 3: even if folding does
  // not settle (x + 1 + 1 + 1 + 1) != (x + 4), unreachable() at i == 4 stops it.
  unsigned x = __VERIFIER_nondet_uint();
  unsigned count = 0;
  for (unsigned i = x; i != x + 4u; ++i)
    ++count;
  __VERIFIER_assert(count == 4u && s == 0);
  return 0;
}

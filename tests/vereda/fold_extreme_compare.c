// A comparison against the least or greatest value of its type folds while
// the walk builds it. The bounds are globals the program never writes, so
// clang leaves the loads, and each check is false before the solver sees it.
// The branches keep <, which InstCombine would turn into >= under a !.
// RUN: --dump-smt-only
// EXPECT: ^\(assert false\)\n\(check-sat\)
extern unsigned __VERIFIER_nondet_uint(void);
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
extern void reach_error(void);
static unsigned zero, umax = ~0u;
static int smin = -2147483647 - 1, smax = 2147483647;
int main(void) {
  unsigned x = __VERIFIER_nondet_uint();
  int y = __VERIFIER_nondet_int();
  __VERIFIER_assert(x >= zero);
  __VERIFIER_assert(x <= umax);
  __VERIFIER_assert(y >= smin);
  __VERIFIER_assert(y <= smax);
  if (x < zero)
    reach_error();
  if (umax < x)
    reach_error();
  if (y < smin)
    reach_error();
  if (smax < y)
    reach_error();
  return 0;
}

// Only the least and greatest values fold: one past either bound is a real
// comparison, and each of these checks fails at the bound.
// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: Properties: 8 checked, \d+ passed, 8 failed, \d+ not checked
extern unsigned __VERIFIER_nondet_uint(void);
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
extern void reach_error(void);
static unsigned one = 1, below = ~0u - 1;
static int above = -2147483647, under = 2147483646;
int main(void) {
  unsigned x = __VERIFIER_nondet_uint();
  int y = __VERIFIER_nondet_int();
  __VERIFIER_assert(x >= one);
  __VERIFIER_assert(x <= below);
  __VERIFIER_assert(y >= above);
  __VERIFIER_assert(y <= under);
  if (x < one)
    reach_error();
  if (below < x)
    reach_error();
  if (y < above)
    reach_error();
  if (under < y)
    reach_error();
  return 0;
}

// SCEV counts the second loop's iterations as x, the nondet call inside the
// first loop, where the second loop reads only 3 * x. The call's own term is
// the one of the last iteration unrolled, not of the one the path left from,
// and no value the second loop reads holds x itself, so the loop is unrolled.
// RUN: --unwind 12 --no-unwinding-assertions
// EXPECT-NOT: Summarised
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
extern unsigned __VERIFIER_nondet_uint(void);
int a[100];
int main(void) {
  unsigned x;
  while (1) {
    x = __VERIFIER_nondet_uint();
    if (x < 30)
      break;
  }
  unsigned y = 3 * x;
  for (unsigned i = 0; i != y; i += 3)
    a[i] = 1;
  __VERIFIER_assert(x <= 10 || a[30] == 1);
  return 0;
}

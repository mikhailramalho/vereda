// RUN: --unwind 30
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: Unwinding loop 0 iteration 10\s
// EXPECT-NOT: Unwinding loop 0 iteration 11\s
// EXPECT: Unwinding loop 1 iteration 4\s
// EXPECT-NOT: Unwinding loop 1 iteration 5\s
// EXPECT: Unwinding loop 2 iteration 4\s
// EXPECT-NOT: Unwinding loop 2 iteration 5\s
// EXPECT: Unwinding loop 3 iteration 6\s
// EXPECT-NOT: Unwinding loop 3 iteration 7\s
// EXPECT-NOT: unwinding assertion
// A loop bounded by an input that an assumption or a branch bounds stops
// unwinding where its test folds by the input's range (#120), well within
// --unwind, so no unwinding assertion is left: signed n <= 10, unsigned
// m < 5, k <= 7 from the else side of a branch (a test of i < k at i = 8,
// which the branch's own test does not settle), and p + 3 with p in [0, 3]
// from a conjunction, compared as a long.
extern int __VERIFIER_nondet_int(void);
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  int n = __VERIFIER_nondet_int(), s = 0;
  __VERIFIER_assume(n <= 10);
  for (int i = 0; i < n; i++)
    s += __VERIFIER_nondet_int() & 1;
  __VERIFIER_assert(s <= 10);

  unsigned m = __VERIFIER_nondet_uint();
  __VERIFIER_assume(m < 5);
  for (unsigned i = 0; i < m; i++)
    s += __VERIFIER_nondet_int() & 1;
  __VERIFIER_assert(s <= 14);

  int k = __VERIFIER_nondet_int();
  if (k > 7)
    return 0;
  for (int i = 0; i < k; i += 2)
    s += __VERIFIER_nondet_int() & 1;
  __VERIFIER_assert(s <= 18);

  int p = __VERIFIER_nondet_int();
  __VERIFIER_assume(p >= 0 && p < 4);
  for (long i = 0; i < p + 3; i++)
    s += __VERIFIER_nondet_int() & 1;
  __VERIFIER_assert(s <= 24);
  return 0;
}

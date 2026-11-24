// RUN: --unwind 15 --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  FAILED +\[main\.assertion\.1\] +line 21
// EXPECT: ^  FAILED +\[main\.assertion\.2\] +line 22
// EXPECT: ^  FAILED +\[main\.assertion\.3\] +line 26
// EXPECT: ^  FAILED +\[main\.unwind\.0\]
// EXPECT: ^  FAILED +\[main\.unwind\.1\]
// Ranges hold only on the path that learned them (#120): n <= 10 on the
// path through the assumption only, so after the join the loop runs past
// 10 iterations, and past the bound; a branch narrows m on one side only.
// A counted assertion narrows nothing, as the path goes on where it fails.
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  int n = __VERIFIER_nondet_int(), s = 0;
  if (__VERIFIER_nondet_int())
    __VERIFIER_assume(n <= 10);
  for (int i = 0; i < n; i++)
    s += __VERIFIER_nondet_int() & 1;
  __VERIFIER_assert(n <= 10);
  __VERIFIER_assert(s <= 10);
  int m = __VERIFIER_nondet_int();
  if (m < 3)
    s = 0;
  __VERIFIER_assert(m < 3);
  for (int i = 0; i < m; i++)
    s += __VERIFIER_nondet_int() & 1;
  __VERIFIER_assert(s <= 40);
  return 0;
}

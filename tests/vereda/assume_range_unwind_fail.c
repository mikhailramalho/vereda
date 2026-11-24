// RUN: --unwind 5
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  FAILED +\[main\.unwind\.0\]
// With n <= 10 the loop may still run past a bound of 5: the range stops
// it at 10 iterations, not earlier, so the unwinding assertion fails (#120).
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  int n = __VERIFIER_nondet_int(), s = 0;
  __VERIFIER_assume(n <= 10);
  for (int i = 0; i < n; i++)
    s += __VERIFIER_nondet_int() & 1;
  __VERIFIER_assert(s <= 10);
  return 0;
}

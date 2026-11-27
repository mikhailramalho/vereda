// Both assertions can fail; the first one is reported, as under
// --incremental-solver, whichever one the solver's model violates.
// EXPECT: FAILED +\[main\.assertion\.1\]
// EXPECT-NOT: \[main\.assertion\.2\]
// EXPECT: ^VERIFICATION FAILED$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x > 10)
    __VERIFIER_assert(x < 100);
  __VERIFIER_assert(x != 5);
  return 0;
}

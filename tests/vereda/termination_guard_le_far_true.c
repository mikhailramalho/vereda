// RUN: --property-file %S/properties/termination.prp --unwind 3
// EXPECT: ^SCEV bounds every loop and no function recurses$
// EXPECT: ^TRUE$
// EXPECT-NOT: Walking
// The guard that rules out n == INT_MAX is two blocks above the loop, past a
// join, so the preheader's own predecessor does not imply it (#481).
extern int __VERIFIER_nondet_int(void);

int main(void) {
  int n = __VERIFIER_nondet_int(), s = 0;
  if (n == 2147483647)
    return 0;
  if (__VERIFIER_nondet_int())
    s = __VERIFIER_nondet_int();
  for (int i = 1; i <= n; i++)
    s++;
  return s;
}

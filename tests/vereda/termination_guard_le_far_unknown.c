// RUN: --property-file %S/properties/termination.prp --unwind 3
// EXPECT: ^UNKNOWN$
// EXPECT-NOT: ^TRUE$
// The guard that rules out n == INT_MAX is on one branch only and does not
// dominate the loop: on the other, with n == INT_MAX, i <= n always holds.
extern int __VERIFIER_nondet_int(void);

int main(void) {
  int n = __VERIFIER_nondet_int(), s = 0;
  if (__VERIFIER_nondet_int()) {
    if (n == 2147483647)
      return 0;
    s = __VERIFIER_nondet_int();
  }
  for (int i = 1; i <= n; i++)
    s++;
  return s;
}

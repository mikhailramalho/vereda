// RUN: --property-file %S/properties/termination.prp --unwind 3
// EXPECT: ^UNKNOWN$
// EXPECT-NOT: ^TRUE$
// The guard rules out INT_MAX - 1, not INT_MAX: with n == INT_MAX, i <= n
// holds for every i and the loop never ends.
extern int __VERIFIER_nondet_int(void);

int main(void) {
  int n = __VERIFIER_nondet_int(), s = 0;
  if (n == 2147483646)
    return 0;
  for (int i = 1; i <= n; i++)
    s++;
  return s;
}

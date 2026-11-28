// RUN: --property-file %S/properties/termination.prp --unwind 3
// EXPECT: ^UNKNOWN$
// EXPECT-NOT: ^TRUE$
// The guard rules out INT_MAX, but the test is unsigned: with n == -1,
// (unsigned)n is UINT_MAX, i <= UINT_MAX always holds and the loop never ends.
extern int __VERIFIER_nondet_int(void);

int main(void) {
  int n = __VERIFIER_nondet_int(), s = 0;
  if (n == 2147483647)
    return 0;
  for (unsigned i = 0; i <= (unsigned)n; i++)
    s++;
  return s;
}

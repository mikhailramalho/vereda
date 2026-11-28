// RUN: --property-file %S/properties/termination.prp --unwind 3
// EXPECT: ^SCEV bounds every loop and no function recurses$
// EXPECT: ^TRUE$
// EXPECT-NOT: Walking
// i <= n ends because the guard rules out n == INT_MAX (loops/sum01_bug02):
// SCEV counts the loop once the exit test is i < n + 1.
extern int __VERIFIER_nondet_int(void);

int main(void) {
  int n = __VERIFIER_nondet_int(), s = 0;
  if (n == 2147483647)
    return 0;
  for (int i = 1; i <= n; i++)
    s++;
  return s;
}

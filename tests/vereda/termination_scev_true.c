// RUN: --property-file %S/properties/termination.prp
// EXPECT: ^SCEV bounds every loop and no function recurses$
// EXPECT: ^TRUE$
// EXPECT-NOT: Walking
// Nested loops with nondet bounds, a count down to zero that wraps, an
// early exit and a call: SCEV bounds each loop without a walk.
extern int __VERIFIER_nondet_int(void);

static int inner(int i) {
  int s = 0;
  for (int j = i; j > 0; j--)
    s++;
  return s;
}

int main(void) {
  int n = __VERIFIER_nondet_int(), s = 0;
  for (int i = 0; i < n; i++)
    s += inner(i);
  for (int k = n; k != 0; k--)
    if (__VERIFIER_nondet_int())
      break;
  return s;
}

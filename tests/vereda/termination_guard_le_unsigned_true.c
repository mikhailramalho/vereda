// RUN: --property-file %S/properties/termination.prp --unwind 3
// EXPECT: ^SCEV bounds every loop and no function recurses$
// EXPECT: ^TRUE$
// EXPECT-NOT: Walking
// The unsigned n >= i, the loop-invariant n on the left, under a guard that
// rules out UINT_MAX: the loop runs n + 1 times.
extern unsigned __VERIFIER_nondet_uint(void);

int main(void) {
  unsigned n = __VERIFIER_nondet_uint(), s = 0;
  if (n < 4294967295u)
    for (unsigned i = 0; n >= i; i++)
      s++;
  return s;
}

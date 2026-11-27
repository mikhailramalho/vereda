// RUN: --property-file %S/properties/termination.prp --unwind 3
// EXPECT: ^UNKNOWN$
// EXPECT-NOT: ^TRUE$
// With n = INT_MAX, i wraps from INT_MAX - 1 to INT_MIN and the loop never
// ends in the walk's arithmetic. C leaves the overflow undefined, and LLVM's
// nsw would let SCEV count the loop, but ir strips the flag.
extern int __VERIFIER_nondet_int(void);

int main(void) {
  int n = __VERIFIER_nondet_int();
  for (int i = 0; i < n; i += 2)
    ;
  return 0;
}

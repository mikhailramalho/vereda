// RUN: --property-file %S/properties/termination.prp --unwind 3
// EXPECT: ^UNKNOWN$
// EXPECT-NOT: ^TRUE$
// Every run ends, as the assumption makes n even, but SCEV does not use the
// assumption and the walk cannot unwind 2^31 iterations: unknown, sound.
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);

int main(void) {
  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(n >= 0 && n % 2 == 0);
  for (int i = 0; i != n; i += 2)
    ;
  return 0;
}

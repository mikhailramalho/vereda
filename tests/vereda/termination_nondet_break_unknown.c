// RUN: --property-file %S/properties/termination.prp --unwind 3
// EXPECT: ^UNKNOWN$
// EXPECT-NOT: ^TRUE$
// The loop ends only when a nondet value says so, which it need not.
extern int __VERIFIER_nondet_int(void);

int main(void) {
  while (1)
    if (__VERIFIER_nondet_int())
      break;
  return 0;
}

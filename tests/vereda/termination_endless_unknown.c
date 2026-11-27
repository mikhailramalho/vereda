// RUN: --property-file %S/properties/termination.prp --unwind 3
// EXPECT: ^UNKNOWN$
// EXPECT-NOT: ^TRUE$
// The walk ends other properties' paths at an endless loop; under
// termination such a path is the violation, so the bound's check fails.
extern int __VERIFIER_nondet_int(void);

int main(void) {
  if (__VERIFIER_nondet_int())
    while (1)
      ;
  return 0;
}

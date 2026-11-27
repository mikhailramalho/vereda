// RUN: --property-file %S/properties/unreach-call.prp --error-label ERROR
// EXPECT: ^  FAILED       \[main\.assertion\.1\]  line 13  error label$
// EXPECT: ^FALSE_REACH$
// Under unreach-call the error label is the error location the property
// names, as in ESBMC's SV-COMP runs, which keep assertions on: reaching it,
// here only when x is 42, is a violation. A failing assertion is not.
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int main(void) {
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assert(x != 7);
  if (x == 42) {
  ERROR:
    return 1;
  }
  return 0;
}

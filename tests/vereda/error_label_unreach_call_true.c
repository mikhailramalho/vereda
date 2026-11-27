// RUN: --property-file %S/properties/unreach-call.prp --error-label ERROR
// EXPECT: ^TRUE$
// Under unreach-call an error label no run reaches is no violation, and the
// code after a reached label of another name still runs.
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x > 0 && x < 0) {
  ERROR:
    return 1;
  }
ERR:
  if (x == 42)
    x = 0;
  return x == 42;
}

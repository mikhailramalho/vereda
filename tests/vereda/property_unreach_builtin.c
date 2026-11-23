// RUN: --property-file %S/properties/unreach-call.prp
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^TRUE$
// A reachable __builtin_unreachable is undefined behaviour, not a call to
// reach_error: under unreach-call it ends the path and is no violation.
extern int __VERIFIER_nondet_int(void);

int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x == 5)
    __builtin_unreachable();
  return 0;
}

// RUN: --property-file %S/properties/unreach-call.prp
// EXPECT: ^Violated property:\n  file \S*property_unreach_false\.c line 12 column 5 function __VERIFIER_assert\n  reach_error$
// EXPECT: ^FALSE_REACH$
// The SV-COMP shape: reach_error has a body that aborts. Under unreach-call
// the call is the violation, so the body is not walked.
#include <assert.h>
extern int __VERIFIER_nondet_int(void);

void reach_error(void) { assert(0); }
void __VERIFIER_assert(int c) {
  if (!c)
    reach_error();
}

int main(void) {
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assert(x != 3);
  return 0;
}

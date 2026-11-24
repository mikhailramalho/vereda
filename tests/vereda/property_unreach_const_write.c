// RUN: --property-file %S/properties/unreach-call.prp
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^TRUE$
// Under unreach-call a write into a string literal is no violation but
// ends its path, as the crash it is: reach_error after it is unreachable.
#include <assert.h>
extern int __VERIFIER_nondet_int(void);

void reach_error(void) { assert(0); }

int main(void) {
  char *p = __VERIFIER_nondet_int() ? "abc" : 0;
  if (p) {
    p[0] = 'x';
    reach_error();
  }
  return 0;
}

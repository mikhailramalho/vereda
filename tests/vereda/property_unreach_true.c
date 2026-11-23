// RUN: --property-file %S/properties/unreach-call.prp
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^TRUE$
// Under unreach-call only reach_error counts: a failed assert aborts the
// path, so reach_error below it is unreachable, and a signed overflow is no
// violation either.
#include <assert.h>
extern int __VERIFIER_nondet_int(void);

void reach_error(void) { assert(0); }

int main(void) {
  int x = __VERIFIER_nondet_int();
  assert(x != 3);
  int y = x + 1;
  if (x == 3)
    reach_error();
  return y;
}

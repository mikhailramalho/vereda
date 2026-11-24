// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// strtol has no body: the call may set errno.
#include <errno.h>
#include <stdlib.h>
extern void __VERIFIER_assert(int);
int main(void) {
  errno = 0;
  long v = strtol("99999999999999999999", 0, 10);
  __VERIFIER_assert(errno == 0);
  return v == 0;
}

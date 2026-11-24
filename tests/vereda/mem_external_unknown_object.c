// RUN: --unwind 2
// EXPECT: ^VERIFICATION UNKNOWN: unsupported: call to read without a body, which may write an object chosen at run time
// A call without a body that gets a pointer loaded from memory may write
// any object, which Vereda does not model.
#include <unistd.h>
extern int __VERIFIER_nondet_int(void);
int main(void) {
  char a[4], b[4];
  char *p[2] = {a, b};
  read(0, p[__VERIFIER_nondet_int() & 1], sizeof a);
  return 0;
}

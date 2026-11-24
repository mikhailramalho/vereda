// RUN: --unwind 2
// EXPECT: ^VERIFICATION SUCCESSFUL$
// A call without a body that gets a pointer loaded from memory may write
// each object whose pointer was stored (#137), not c, whose address the
// program keeps.
#include <unistd.h>
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int main(void) {
  char a[4], b[4], c[4] = "abc";
  char *p[2] = {a, b};
  read(0, p[__VERIFIER_nondet_int() & 1], sizeof a);
  __VERIFIER_assert(c[0] == 'a');
  return 0;
}

// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// A call without a body that gets a pointer loaded from memory may write
// the object it was stored from (#137).
#include <unistd.h>
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int main(void) {
  char a[4] = "abc", b[4] = "abc";
  char *p[2] = {a, b};
  read(0, p[__VERIFIER_nondet_int() & 1], sizeof a);
  __VERIFIER_assert(a[0] == 'a' && b[0] == 'a');
  return 0;
}

// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// A call without a body writes the object that a pointer chosen by a branch
// points to.
#include <unistd.h>
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
int main(void) {
  char a[4] = "", b[4] = "";
  int c = __VERIFIER_nondet_int();
  read(0, c ? a : b, sizeof a);
  if (c)
    __VERIFIER_assert(a[0] == 0);
  return 0;
}

// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// sscanf has no body: the call may write x, so x == 5 is reachable.
#include <stdio.h>
extern void __VERIFIER_assert(int);
int main(void) {
  char buf[8] = "5";
  int x = 0;
  sscanf(buf, "%d", &x);
  __VERIFIER_assert(x != 5);
  return 0;
}

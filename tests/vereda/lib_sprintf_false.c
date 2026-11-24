// The digits sprintf writes for a nondet int are its digits: the solver
// finds the value that prints as "42".
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: line 12
#include <stdio.h>
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
int main(void) {
  char b[12];
  int x = __VERIFIER_nondet_int();
  sprintf(b, "%d", x);
  __VERIFIER_assert(!(b[0] == '4' && b[1] == '2' && b[2] == 0));
  return 0;
}

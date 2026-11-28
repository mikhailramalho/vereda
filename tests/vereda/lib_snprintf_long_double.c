// snprintf takes a long double for %Lf and the argument after it from the
// right place (#305): %n stores the count, 1 for '<' and between 1 and
// 4933 + 6 + 2 characters for the number.
// The modeled formatting loop can exceed the default incremental bound.
// RUN: -m64 --no-incremental-bmc
// RUN: -m32 --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdio.h>
extern long double __VERIFIER_nondet_longdouble(void);
extern void __VERIFIER_assert(int);

int main(void) {
  char buf[4];
  int pos = -1;
  int r = snprintf(buf, sizeof buf, "<%Lf%n", __VERIFIER_nondet_longdouble(), &pos);
  __VERIFIER_assert(pos >= 2 && pos <= 4942);
  __VERIFIER_assert(r == pos);
  __VERIFIER_assert(buf[0] == '<');
  return 0;
}

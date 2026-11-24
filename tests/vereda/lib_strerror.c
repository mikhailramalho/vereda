// strerror gives a non-empty message for any error number.
// RUN: --unwind 16
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int main(void) {
  int e = __VERIFIER_nondet_int();
  char *m = strerror(e);
  __VERIFIER_assert(m != NULL && m[0] != 0 && strlen(m) < 64);
  return 0;
}

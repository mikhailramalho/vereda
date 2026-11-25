// Failing counterpart for mergeSeparate: an object separated on one branch
// only retains the other branch's value when that branch is taken.
// EXPECT: ^VERIFICATION FAILED$
#include <string.h>

extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern int __VERIFIER_nondet_int(void);
extern unsigned __VERIFIER_nondet_uint(void);

int main(void) {
  static unsigned char become_sep[1024];
  unsigned idx = __VERIFIER_nondet_uint();
  __VERIFIER_assume(idx < 1024);
  int c = __VERIFIER_nondet_int();

  if (c)
    memset(become_sep, 31, sizeof(become_sep));
  else
    become_sep[idx] = 32;

  __VERIFIER_assert(become_sep[idx] == 31);
  return 0;
}

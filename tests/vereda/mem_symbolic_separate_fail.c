// Symbolic-pointer write through a mix of separate and shared objects must
// detect when the other object was not modified.
// EXPECT: ^VERIFICATION FAILED$
#include <string.h>

extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern int __VERIFIER_nondet_int(void);
extern unsigned __VERIFIER_nondet_uint(void);

int main(void) {
  static unsigned char sep1[1024];
  unsigned char shared_buf[8] = {10, 11, 12, 13, 14, 15, 16, 17};
  memset(sep1, 1, sizeof(sep1));

  unsigned idx = __VERIFIER_nondet_uint();
  __VERIFIER_assume(idx < 8);
  int c = __VERIFIER_nondet_int();

  unsigned char *q = c ? sep1 : shared_buf;
  q[idx] = 99;
  // Fails when c == 0 (only shared_buf was written, sep1[idx] is still 1).
  __VERIFIER_assert(sep1[idx] == 99);
  return 0;
}

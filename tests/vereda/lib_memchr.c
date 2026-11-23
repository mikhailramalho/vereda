// RUN: --unwind 2
// The walker finds memchr's match over the whole range (#67): --unwind 2
// would stop a byte loop long before 32. c is converted to unsigned char,
// and n may be symbolic.
#include <stddef.h>
#include <string.h>
extern unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  char s[32];
  memset(s, 'x', sizeof s);
  s[20] = 'y';
  s[25] = 'y';
  assert(memchr(s, 'y', 32) == s + 20);
  assert(memchr(s, 'y', 20) == NULL);
  assert(memchr(s, 'z', 32) == NULL);
  assert(memchr(s, 'y' + 256, 32) == s + 20);
  unsigned n = __VERIFIER_nondet_uint() % 33;
  assert(memchr(s, 'y', n) == (n > 20 ? s + 20 : NULL));
  return 0;
}
// EXPECT: ^VERIFICATION SUCCESSFUL$

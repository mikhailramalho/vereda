// RUN: --unwind 2
// lib_memchr's results are not vacuous: a 'y' at 20 is found for some n.
#include <stddef.h>
#include <string.h>
extern unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  char s[32];
  memset(s, 'x', sizeof s);
  s[20] = 'y';
  unsigned n = __VERIFIER_nondet_uint() % 33;
  assert(memchr(s, 'y', n) == NULL);
  return 0;
}
// EXPECT: ^VERIFICATION FAILED$
// EXPECT-NOT: unwinding assertion

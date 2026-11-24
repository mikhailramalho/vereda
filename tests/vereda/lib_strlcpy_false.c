// RUN: --unwind 12
// EXPECT: ^VERIFICATION FAILED$
#include <string.h>
extern size_t strlcpy(char *, const char *, size_t);
extern char __VERIFIER_nondet_char(void);
extern void __VERIFIER_assert(int);

int main(void) {
  char c = __VERIFIER_nondet_char();
  char s[4] = {c, 'b', 'c', 0};
  char a[3];

  size_t n = strlcpy(a, s, 3);
  __VERIFIER_assert(n == 3);

  return 0;
}

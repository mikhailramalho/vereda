// strndup(s, 40) of a string of up to 63 nondet characters keeps at most
// 40 of them, so the 41st byte of the copy is the terminator, which
// differs from the source when it is longer than 40.
// EXPECT: ^VERIFICATION FAILED$
#include <stdlib.h>
#include <string.h>
extern void __VERIFIER_assert(int);
extern char __VERIFIER_nondet_char(void);

int main(void) {
  char s[64];
  for (int i = 0; i < 63; i++)
    s[i] = __VERIFIER_nondet_char();
  s[63] = 0;
  char *e = strndup(s, 40);
  if (e && strlen(s) >= 40)
    __VERIFIER_assert(e[40] == s[40]);
  return 0;
}

// sprintf writes the formatted text and its NUL and returns the length;
// snprintf writes at most n - 1 bytes and the NUL, leaves the rest alone,
// and returns the length the whole text has (C11 7.21.6.5); asprintf
// allocates exactly that length and the NUL.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
int main(void) {
  char b[8];
  __VERIFIER_assert(sprintf(b, "%s=%+d|%c", "ab", 7, 'z') == 7); // exact fit
  __VERIFIER_assert(strcmp(b, "ab=+7|z") == 0);

  char c[8] = "abcdefg";
  int n = snprintf(c, 4, "%x:%05u", 255u, 42u);
  __VERIFIER_assert(n == 8);
  __VERIFIER_assert(c[0] == 'f' && c[1] == 'f' && c[2] == ':' && c[3] == 0 && c[4] == 'e');
  __VERIFIER_assert(snprintf(NULL, 0, "%d", -1234) == 5);

  int x = __VERIFIER_nondet_int();
  n = snprintf(b, sizeof b, "%d", x);
  __VERIFIER_assert(n >= 1 && n <= 11);
  __VERIFIER_assert(x != -5 || (n == 2 && b[0] == '-' && b[1] == '5' && b[2] == 0));

  char *s;
  if (asprintf(&s, "[%3s]", "x") == 5)
    __VERIFIER_assert(strcmp(s, "[  x]") == 0);
  return 0;
}

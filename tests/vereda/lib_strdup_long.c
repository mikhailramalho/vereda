// strdup and strndup copy a string of up to 63 nondet characters: every
// byte up to the terminator matches, and strndup's copy is cut at 40
// characters and terminated.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdlib.h>
#include <string.h>
extern void __VERIFIER_assert(int);
extern char __VERIFIER_nondet_char(void);
extern unsigned __VERIFIER_nondet_uint(void);

int main(void) {
  char s[64];
  for (int i = 0; i < 63; i++)
    s[i] = __VERIFIER_nondet_char();
  s[63] = 0;
  size_t n = strlen(s);
  unsigned k = __VERIFIER_nondet_uint();

  char *d = strdup(s);
  if (d && k <= n)
    __VERIFIER_assert(d[k] == s[k]);

  char *e = strndup(s, 40);
  if (e && k <= n && k <= 40)
    __VERIFIER_assert(e[k] == (k < 40 ? s[k] : 0));
  return 0;
}

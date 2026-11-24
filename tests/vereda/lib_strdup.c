// RUN: --unwind 8
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdlib.h>
#include <string.h>
extern void __VERIFIER_assert(int);
extern char __VERIFIER_nondet_char(void);

int main(void) {
  char s[4] = {__VERIFIER_nondet_char(), 'b', 'c', 0};
  char *d = strdup(s);
  if (d != NULL) {
    __VERIFIER_assert(d != s);
    __VERIFIER_assert(strcmp(d, s) == 0);
    __VERIFIER_assert(strlen(d) == strlen(s));
    char c = s[0];
    d[0] = 'x';
    __VERIFIER_assert(s[0] == c); // the copy is a separate block
    free(d);
  }

  char *e = strndup(s, 2);
  if (e != NULL) {
    __VERIFIER_assert(strlen(e) == (s[0] == 0 ? 0 : 2));
    if (s[0] != 0) {
      __VERIFIER_assert(e[0] == s[0]);
      __VERIFIER_assert(e[1] == 'b');
    }
    free(e);
  }

  return 0;
}
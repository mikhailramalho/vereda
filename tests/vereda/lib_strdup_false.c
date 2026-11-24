// strndup(s, 2) is empty, not two characters long, when s[0] is 0.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION FAILED$
#include <stdlib.h>
#include <string.h>
extern void __VERIFIER_assert(int);
extern char __VERIFIER_nondet_char(void);

int main(void) {
  char s[4] = {__VERIFIER_nondet_char(), 'b', 'c', 0};
  char *e = strndup(s, 2);
  if (e)
    __VERIFIER_assert(strlen(e) == 2);
  return 0;
}

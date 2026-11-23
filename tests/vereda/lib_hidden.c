// A counterexample through the library models shows the program's
// assignments but none from inside a model, as ESBMC hides its library.
// RUN: --unwind 6
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^State 1 file \S+ line \d+( column \d+)? function main
// EXPECT: up = 0
// EXPECT-NOT: function (strcpy|strlen|realloc|toupper|memcpy|malloc) thread
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
extern void __VERIFIER_assert(int);
extern char __VERIFIER_nondet_char(void);
int main(void) {
  char s[4] = {'a', __VERIFIER_nondet_char(), 'c', 0};
  char *d = malloc(8);
  if (!d)
    return 0;
  strcpy(d, s);
  d = realloc(d, 16);
  if (!d)
    return 0;
  int up = toupper(d[1]);
  __VERIFIER_assert(up != 0);
  return 0;
}

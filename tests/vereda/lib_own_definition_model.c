// A program's own malloc, which only the strdup model calls, is the one that
// runs: dropping it as unreachable before the models are linked would let
// the walker's malloc take its place and hide the violation.
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: main.assertion.1
#include <stddef.h>
#include <string.h>
extern char __VERIFIER_nondet_char(void);
extern void __VERIFIER_assert(int);
static int called;
void *malloc(size_t n) {
  (void)n;
  called = 1;
  return NULL;
}
int main(void) {
  char s[2] = {__VERIFIER_nondet_char(), 0};
  char *p = strdup(s);
  __VERIFIER_assert(!called);
  return p ? p[0] : 0;
}

// The strstr model: it should return a pointer to the first occurrence of needle in haystack,
// or NULL if none, or haystack itself when needle is empty.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern void __VERIFIER_assert(int);
extern char __VERIFIER_nondet_char(void);
int main(void) {
  char s[4] = {__VERIFIER_nondet_char(), 'b', 'c', 0};
  char *p = strstr(s, "bc");
  __VERIFIER_assert(p == s + 1);
  return 0;
}
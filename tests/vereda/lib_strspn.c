// strspn, strcspn and strpbrk on a string whose first character is nondet, so
// that clang cannot fold the calls and the models run.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern char __VERIFIER_nondet_char(void);
extern void __VERIFIER_assert(int);

int main(void) {
  char s[4];
  s[0] = __VERIFIER_nondet_char();
  s[1] = 'b';
  s[2] = 'c';
  s[3] = 0;

  __VERIFIER_assert(strspn(s, "ab") == ((s[0] == 'a' || s[0] == 'b') ? 2 : 0));
  __VERIFIER_assert(strcspn(s, "c") == ((s[0] == 'c' || s[0] == 0) ? 0 : 2));
  __VERIFIER_assert(strpbrk(s, "c") == (s[0] == 0 ? NULL : s[0] == 'c' ? s : s + 2));

  return 0;
}

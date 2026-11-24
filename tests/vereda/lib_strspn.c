// Check strspn, strcspn and strpbrk on a string whose first character is nondet.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern char __VERIFIER_nondet_char(void);
extern void __VERIFIER_assert(int);

int main(void) {
  // Create a local array with non-deterministic input that cannot be folded at compile time
  char s[4];
  s[0] = __VERIFIER_nondet_char();
  s[1] = 'b';
  s[2] = 'c';
  s[3] = 0;

  // For strspn(s, "ab"):
  // - If s[0] is 'a' or 'b', return 2 (since we match s[0] and s[1])
  // - Otherwise, return 0 (we stop immediately when first character doesn't match)
  __VERIFIER_assert(strspn(s, "ab") == ((s[0] == 'a' || s[0] == 'b') ? 2 : 0));

  // For strcspn(s, "c"):
  // - If s[0] is null terminator or 'c', return 0
  // - Otherwise, return 2 (since we match s[1] and s[2] are constants)
  __VERIFIER_assert(strcspn(s, "c") == ((s[0] == 'c' || s[0] == 0) ? 0 : 2));

  // For strpbrk(s, "c"):
  // - If s[0] is null terminator, return NULL
  // - Else if s[0] is 'c', return s (pointer to first char)
  // - Otherwise, return s+2 (since s[2] is 'c')
  __VERIFIER_assert(strpbrk(s, "c") == (s[0] == 0 ? NULL : s[0] == 'c' ? s : s + 2));

  return 0;
}
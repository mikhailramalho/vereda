// Test strspn, strcspn and strpbrk functions.
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

  // C17 7.24.5.6 strspn tests
  // For strspn(s, "ab"):
  // - If s[0] is 'a' or 'b', return 2 (since we match s[0] and s[1])
  // - Otherwise, return 0 (we stop immediately when first character doesn't match)
  __VERIFIER_assert(strspn(s, "ab") == ((s[0] == 'a' || s[0] == 'b') ? 2 : 0));

  // C17 7.24.5.3 strcspn tests
  // For strcspn(s, "c"):
  // This returns the number of characters from start until we find a character in "c"
  // The model implementation:
  //   for (; s[n]; ++n) {
  //     const char *r;
  //     for (r = reject; *r && *r != s[n]; ++r)
  //       ;
  //     if (*r)
  //       break;
  //   }
  //   return n;
  // So:
  // - If s[0] is null terminator, we don't enter the loop at all and return 0
  // - Else if s[0] is 'c', we find the match immediately at position 0, so return 0
  // - Else if s[1] is 'c', we find it at position 1, so return 1
  // - Else if s[2] is 'c', we find it at position 2, so return 2
  // - Otherwise (no match found), return 3 (traverse all until null terminator)
  __VERIFIER_assert(strcspn(s, "c") == ((s[0] == 0) ? 0 : ((s[0] == 'c') ? 0 : ((s[1] == 'c') ? 1 : ((s[2] == 'c') ? 2 : 3)))));

  // C17 7.24.5.4 strpbrk tests
  // For strpbrk(s, "c"):
  // This returns pointer to first character in s that matches any character in "c"
  // The model implementation:
  //   while (*s) { // Only continue if current character is not null terminator
  //     const char *r;
  //     for (r = reject; *r && *r != *s; ++r)
  //       ;
  //     if (*r)
  //       return (char*) s;
  //     ++s;
  //   }
  //   return NULL;
  // So:
  // - If s[0] is null terminator, we don't enter the loop at all and return NULL
  // - Else if s[0] is 'c', return s (pointer to first char)
  // - Else if s[1] is 'c', return s+1 (pointer to second char)
  // - Else if s[2] is 'c', return s+2 (pointer to third char)
  // - Otherwise, return NULL
  __VERIFIER_assert(strpbrk(s, "c") == ((s[0] == 0) ? NULL : ((s[0] == 'c') ? s : ((s[1] == 'c') ? s + 1 : ((s[2] == 'c') ? s + 2 : NULL)))));

  return 0;
}
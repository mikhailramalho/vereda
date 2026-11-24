// Simple strstr test focusing only on basic behavior.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern void __VERIFIER_assert(int);
extern char __VERIFIER_nondet_char(void);
int main(void) {
  // Test case where we have a known string with the needle at position 1
  char s[4] = {'x', 'b', 'c', 0};
  char *p = strstr(s, "bc");
  __VERIFIER_assert(p == s + 1);  // Should find "bc" starting at index 1
  return 0;
}
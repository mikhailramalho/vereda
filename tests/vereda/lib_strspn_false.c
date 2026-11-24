// Test that strcspn with wrong assertion fails.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION FAILED$
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

  // This should fail - strcspn(s, "dx") is not always 4
  // For strcspn(s, "dx"):
  // - If s[0] is 'd' or 'x', return 0
  // - Else if s[1] is 'd' or 'x', return 1
  // - Else if s[2] is 'd' or 'x', return 2
  // - Otherwise, return 3
  __VERIFIER_assert(strcspn(s, "dx") == 4);

  return 0;
}

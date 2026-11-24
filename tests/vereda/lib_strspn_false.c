// Check strcspn with wrong assertion that fails only for specific inputs.
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

  // This should fail - strcspn(s, "c") is not always 2
  // It only fails when s[0] is 'c' or null terminator (which gives 0)
  __VERIFIER_assert(strcspn(s, "c") == 2);

  return 0;
}

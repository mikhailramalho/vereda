// Test that strcspn with wrong assertion fails.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION FAILED$
#include <string.h>
extern void __VERIFIER_assert(int);
int main(void) {
  // This should fail - strcspn("abcde", "dx") is 3, not 4
  __VERIFIER_assert(strcspn("abcde", "dx") == 4);
  return 0;
}
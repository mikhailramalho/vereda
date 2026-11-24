// Test strspn, strcspn and strpbrk functions.
// RUN: --unwind 8
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <string.h>
extern void __VERIFIER_assert(int);
int main(void) {
  // C17 7.24.5.6 strspn tests
  __VERIFIER_assert(strspn("abcde", "cba") == 3);
  __VERIFIER_assert(strspn("xyz", "abc") == 0);

  // C17 7.24.5.3 strcspn tests
  __VERIFIER_assert(strcspn("abcde", "dx") == 3);
  __VERIFIER_assert(strcspn("abc", "") == 3);

  // C17 7.24.5.4 strpbrk tests
  __VERIFIER_assert(strpbrk("hello", "lo") == &"hello"[2]);
  __VERIFIER_assert(strpbrk("abc", "xyz") == NULL);

  return 0;
}
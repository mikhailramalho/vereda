// The fourth byte keeps its value only when the nondet size is below 4.
// EXPECT: ^  FAILED       \[main\.assertion\.1\]  line 14  __VERIFIER_assert$
// EXPECT: ^VERIFICATION FAILED$
#include <string.h>
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

int main(void) {
  char buf[8] = {1, 2, 3, 4, 5, 6, 7, 8};
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n >= 1 && n <= 4);
  memset(buf, 0, n);
  __VERIFIER_assert(buf[3] == 4);
  return 0;
}

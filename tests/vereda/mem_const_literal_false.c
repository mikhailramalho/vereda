// A write into a string literal is undefined (C11 6.4.5p7). Here memset
// writes through a pointer to a local array or to the literal, and fails
// where it is the literal, worded as ESBMC words it.
// EXPECT: ^  dereference failure: write access to string constant$
// EXPECT: ^  FAILED       \[main\.assertion\.1\]  line 15  dereference failure: write access to string constant$
// EXPECT: ^VERIFICATION FAILED$
#include <string.h>

extern int __VERIFIER_nondet_int(void);

int main(void) {
  char buf[4];
  int x = __VERIFIER_nondet_int();
  char *p = x > 0 ? buf : "abc";
  memset(p, 0, 2);
  return 0;
}

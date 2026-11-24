// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// %3ms sets p to a buffer sscanf allocates: all of the pointer is written,
// not the 4 bytes a width of 3 gives %s.
#include <stdio.h>
extern void __VERIFIER_assert(int);
int main(void) {
  char s[8];
  char *p = 0;
  sscanf(s, "%3ms", &p);
  __VERIFIER_assert(((unsigned char *)&p)[sizeof p - 1] == 0);
  return 0;
}

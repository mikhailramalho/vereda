// RUN: --unwind 2
// EXPECT: ^VERIFICATION SUCCESSFUL$
// errno is zero at program start, and a call without a body whose attributes
// say it only reads memory (atoi) or writes only its arguments' (frexp)
// leaves errno and the library's globals alone.
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
extern void __VERIFIER_assert(int);
int main(void) {
  __VERIFIER_assert(errno == 0);
  char a[4] = "12";
  int e;
  errno = 7;
  optind = 2;
  int n = atoi(a);
  frexp(1.5, &e);
  __VERIFIER_assert(errno == 7);
  __VERIFIER_assert(optind == 2);
  return n;
}

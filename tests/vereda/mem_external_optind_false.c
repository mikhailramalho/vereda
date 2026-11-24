// RUN: --unwind 2
// EXPECT: ^VERIFICATION FAILED$
// getopt has no body: the call may change optind, a global the C library
// defines.
#include <unistd.h>
extern void __VERIFIER_assert(int);
int main(void) {
  char a0[] = "p", a1[] = "-a";
  char *args[] = {a0, a1, 0};
  optind = 1;
  getopt(2, args, "a");
  __VERIFIER_assert(optind == 1);
  return 0;
}

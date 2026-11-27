// popen gives a new stream object, or null. pclose frees it, and fclose of
// a popen stream (unspecified by POSIX) frees it too, as for fopen.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdio.h>
extern char __VERIFIER_nondet_char(void);
extern void __VERIFIER_assert(int);
int main(void) {
  FILE *a = popen("ls", "r");
  FILE *b = popen("cat", "w");
  if (a && b)
    __VERIFIER_assert(a != b);
  if (a)
    pclose(a);
  if (b) {
    fputc(__VERIFIER_nondet_char(), b);
    fclose(b);
  }
  return 0;
}

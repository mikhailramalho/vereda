// pclose of a stream popen did not open is undefined (POSIX): reported,
// here only when c is 'q'. pclose of a popen stream written to is not.
// EXPECT: ^VERIFICATION FAILED$
#include <stdio.h>
extern char __VERIFIER_nondet_char(void);
int main(void) {
  char c = __VERIFIER_nondet_char();
  FILE *p = popen("ls", "r");
  if (p) {
    fputc(c, p);
    pclose(p);
  }
  FILE *f = fopen("x", "r");
  if (f && c == 'q')
    pclose(f);
  return 0;
}
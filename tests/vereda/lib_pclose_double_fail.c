// pclose frees a popen stream, so an fclose after it is a double free,
// here only when c is 'q'.
// EXPECT: ^  FAILED       \[main\.invalidated-dynamic-object-freed\.1\]  line 14  dereference failure: invalidated dynamic object freed$
// EXPECT: ^VERIFICATION FAILED$
#include <stdio.h>
extern char __VERIFIER_nondet_char(void);
int main(void) {
  char c = __VERIFIER_nondet_char();
  FILE *p = popen("ls", "r");
  if (!p)
    return 0;
  pclose(p);
  if (c == 'q')
    fclose(p);
  return 0;
}
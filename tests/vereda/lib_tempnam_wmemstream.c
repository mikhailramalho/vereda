// tempnam allocates the name it returns, and after fclose *ptr of an
// open_wmemstream stream is a buffer the program frees, so free takes
// both (#503).
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
extern int __VERIFIER_nondet_int(void);

int main(void) {
  free(tempnam(NULL, "x"));
  free(tempnam("/tmp", NULL));
  wchar_t *p;
  size_t n;
  FILE *f = open_wmemstream(&p, &n);
  if (!f)
    return 0;
  fputwc(__VERIFIER_nondet_int(), f);
  fclose(f);
  free(p);
  return 0;
}

// tempnam's name and open_wmemstream's buffer are heap objects: freeing
// either twice is a double free, each only where the call succeeds (#503).
// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  FAILED +\[main\.invalidated-dynamic-object-freed\.1\]
// EXPECT: ^  FAILED +\[main\.invalidated-dynamic-object-freed\.5\]
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

int main(void) {
  char *t = tempnam(NULL, "x");
  free(t);
  free(t);
  wchar_t *p = 0;
  size_t n = 0;
  FILE *f = open_wmemstream(&p, &n);
  if (!f)
    return 0;
  fclose(f);
  free(p);
  free(p);
  return 0;
}

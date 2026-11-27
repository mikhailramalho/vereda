// open_memstream's buffer is a heap object: freeing it twice after fclose
// is a double free.
// EXPECT: ^  FAILED       \[main\.invalidated-dynamic-object-freed\.\d+\]  line 16  dereference failure: invalidated dynamic object freed$
// EXPECT: ^VERIFICATION FAILED$
#include <stdio.h>
#include <stdlib.h>
int main(void) {
  char *p = 0;
  size_t n = 0;
  FILE *o = open_memstream(&p, &n);
  if (!o)
    return 0;
  fputs("hello", o);
  fclose(o);
  free(p);
  free(p);
  return 0;
}

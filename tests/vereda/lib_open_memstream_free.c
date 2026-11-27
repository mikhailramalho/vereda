// After fclose, *ptr of an open_memstream stream is a buffer the program
// frees (POSIX), not a pointer the writes to the stream made unknown.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <stdio.h>
#include <stdlib.h>
extern char __VERIFIER_nondet_char(void);
int main(void) {
  char *p = 0;
  size_t n = 0;
  FILE *o = open_memstream(&p, &n);
  if (!o)
    return 0;
  fputc(__VERIFIER_nondet_char(), o);
  fclose(o);
  free(p);
  return 0;
}

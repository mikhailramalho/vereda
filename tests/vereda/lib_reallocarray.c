// EXPECT: ^VERIFICATION SUCCESSFUL$
// reallocarray fails with ENOMEM where n * size overflows and leaves p as
// it was (2^62 * 8 wraps to 0, which realloc would take as a free), and
// otherwise is realloc of the product (#358).
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
int main(void) {
  int *p = malloc(sizeof(int));
  int *q = reallocarray(p, (size_t)1 << 62, 8);
  assert(!q && errno == ENOMEM);
  free(p);
  int *r = reallocarray(NULL, 3, sizeof(int));
  if (r)
    r[2] = 1;
  free(r);
  return 0;
}

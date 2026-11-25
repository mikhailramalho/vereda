// EXPECT: ^VERIFICATION FAILED$
#include <assert.h>
#include <string.h>

extern void unknown_fn(void *p);

int main(void) {
  int x = 10;
  int *src[256], *dst[256];
  src[0] = &x;
  memcpy(dst, src, sizeof(dst));
  // unknown_fn receives dst, which holds a copied pointer to x, so x may be modified.
  unknown_fn(dst);
  assert(x == 10);
  return 0;
}

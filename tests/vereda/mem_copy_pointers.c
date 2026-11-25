// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <assert.h>
#include <string.h>

extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);

int main(void) {
  int x = 10, y = 20;
  int *src[256], *dst[256];
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 256);
  src[0] = &y;
  src[i] = &x;
  memcpy(dst, src, sizeof(dst));
  *dst[i] = 30;
  assert(x == 30);
  if (i != 0)
    assert(*dst[0] == 20);
  return 0;
}

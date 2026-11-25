// EXPECT: ^VERIFICATION FAILED$
#include <assert.h>
#include <string.h>

extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);

int main(void) {
  int src[1024], dst[1024];
  int k = __VERIFIER_nondet_int();
  __VERIFIER_assume(k >= 0 && k < 512);
  dst[256 + k] = 111;
  src[k] = 77;
  memcpy(&dst[256], &src[0], 512 * sizeof(int));
  assert(dst[256 + k] == 111);
  return 0;
}

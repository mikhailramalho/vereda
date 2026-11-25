// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <assert.h>
#include <string.h>

extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);

int main(void) {
  int src[1024], dst[1024];
  int k = __VERIFIER_nondet_int();
  __VERIFIER_assume(k >= 0 && k < 512);
  dst[10] = 111;
  dst[900] = 222;
  src[k] = 77;
  // Partial copy of 2048 bytes (512 ints) into the middle of dst.
  memcpy(&dst[256], &src[0], 512 * sizeof(int));
  assert(dst[10] == 111);
  assert(dst[900] == 222);
  assert(dst[256 + k] == 77);
  return 0;
}

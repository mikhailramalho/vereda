// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <assert.h>
#include <string.h>

extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);

int main(void) {
  int buf[1024];
  int k = __VERIFIER_nondet_int();
  __VERIFIER_assume(k >= 0 && k < 512);
  buf[k] = 12345;
  buf[1000] = 99;
  // Forward overlap (dst > src): copy 512 ints (2048 bytes) up by 128 ints.
  memmove(&buf[128], &buf[0], 512 * sizeof(int));
  assert(buf[128 + k] == 12345);
  assert(buf[1000] == 99);
  // Backward overlap (dst < src): copy 512 ints (2048 bytes) down by 64 ints.
  memmove(&buf[64], &buf[128], 512 * sizeof(int));
  assert(buf[64 + k] == 12345);
  assert(buf[1000] == 99);
  return 0;
}

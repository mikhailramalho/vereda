// EXPECT: ^VERIFICATION FAILED$
#include <assert.h>
#include <string.h>

extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);

int main(void) {
  int buf[1024];
  int k = __VERIFIER_nondet_int();
  __VERIFIER_assume(k >= 0 && k < 512);
  buf[k] = 12345;
  buf[128 + k] = 54321;
  memmove(&buf[128], &buf[0], 512 * sizeof(int));
  assert(buf[128 + k] == 54321);
  return 0;
}

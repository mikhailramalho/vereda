// EXPECT: ^VERIFICATION FAILED$
#include <assert.h>
#include <string.h>

extern int __VERIFIER_nondet_int(void);
extern _Bool __VERIFIER_nondet_bool(void);
extern void __VERIFIER_assume(int);

int main(void) {
  int src[512], dst[512];
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 512);
  src[i] = 5;
  dst[i] = 9;
  _Bool c = __VERIFIER_nondet_bool();
  if (c)
    memcpy(dst, src, sizeof(dst));
  assert(dst[i] == 9);
  return 0;
}

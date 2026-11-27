#include <assert.h>
#include <stddef.h>
union chunk { unsigned char bytes[4]; };
size_t __VERIFIER_nondet_size(void);
int main(void)
{
  unsigned int x = 0x11223344u;
  size_t off = __VERIFIER_nondet_size();
  __VERIFIER_assume(off < 2);
  union chunk *p = (union chunk *)((unsigned char *)&x + off);
  union chunk a = *p;
  assert(a.bytes[0] == 0x44);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10939_fail
// EXPECT: ^  dereference failure: Access to object out of bounds$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  dereference failure: Access to object out of bounds$
// MODE: FUTURE
// Vereda: verdict FAILED

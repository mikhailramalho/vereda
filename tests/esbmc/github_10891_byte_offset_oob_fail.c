/* #10891 */
#include <stdint.h>
unsigned __VERIFIER_nondet_uint(void);
struct __attribute__((packed)) U { uint32_t v; };
int main(void)
{
  uint32_t buf[2] = {1, 2};
  unsigned o = __VERIFIER_nondet_uint();
  __VERIFIER_assume(o <= 5);
  struct U r = *(struct U *)((unsigned char *)buf + o);
  return r.v;
}
// Imported from ESBMC regression/esbmc/github_10891_byte_offset_oob_fail
// EXPECT: ^  dereference failure: array bounds violated$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  dereference failure: array bounds violated$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL

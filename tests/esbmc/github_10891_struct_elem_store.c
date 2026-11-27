/* #10891 */
#include <assert.h>
#include <stdint.h>
unsigned __VERIFIER_nondet_uint(void);
struct MU { uint32_t value; };
struct __attribute__((packed)) Unaligned { uint32_t v; };
int main(void)
{
  struct MU buf[2];
  unsigned o = __VERIFIER_nondet_uint();
  __VERIFIER_assume(o <= sizeof(buf) - sizeof(uint32_t));
  struct Unaligned *p = (struct Unaligned *)((unsigned char *)buf + o);
  struct Unaligned val = {7};
  *p = val;
  struct Unaligned r = *p;
  assert(r.v == 7);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10891_struct_elem_store
// EXPECT: ^VERIFICATION SUCCESSFUL$

#include <assert.h>
#include <stdint.h>
typedef struct
{
  uint32_t cfg;
  uint32_t shadow[8];
  uint32_t tail;
} state_t;
static state_t g;
int main(void)
{
  unsigned k = __VERIFIER_nondet_uint();
  _Bool c = __VERIFIER_nondet_bool();
  __VERIFIER_assume(k < 8);
  uint32_t *p = c ? &g.shadow[k] : &g.tail;
  *p = 5;
  assert(c || g.tail == 5);
  assert(!c || g.shadow[k] == 5);
  return 0;
}
// Imported from ESBMC regression/esbmc/array_member_merge
// EXPECT: ^VERIFICATION SUCCESSFUL$

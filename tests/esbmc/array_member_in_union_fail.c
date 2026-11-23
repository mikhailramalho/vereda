#include <assert.h>
#include <stdint.h>
typedef struct
{
  uint32_t cfg;
  union
  {
    uint32_t a[8];
    uint64_t b[4];
  } u;
  uint32_t tail;
} state_t;
static state_t g;
int main(void)
{
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k < 8);
  g.tail = 0;
  uint32_t *p = &g.u.a[k];
  p[8 - k] = 42; /* g.tail */
  assert(g.tail == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/array_member_in_union_fail
// EXPECT: ^VERIFICATION FAILED$

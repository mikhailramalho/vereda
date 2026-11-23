#include <assert.h>

struct S
{
  unsigned x : 3;
} s;

int main(void)
{
  s.x = __VERIFIER_nondet_uint();
  unsigned r = s.x++;
  assert(r != 7);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9443_fail
// EXPECT: ^VERIFICATION FAILED$

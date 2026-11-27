#include <assert.h>
#include <string.h>
struct S
{
  char name[8];
  int x;
} s;
int main(void)
{
  s.x = 5;
  unsigned n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n <= sizeof s);
  memset(&s, 0, n);
  assert(s.x == 5);
  return 0;
}
// Imported from ESBMC regression/esbmc/array_member_memset_walk_fail
// RUN: --unwind 13
// EXPECT: ^VERIFICATION FAILED$

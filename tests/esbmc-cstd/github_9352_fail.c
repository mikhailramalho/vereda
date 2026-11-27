// #9352: (_Bool)(1 + 1) is 1, so the post-increment leaves s.b set.
#include <assert.h>

struct S
{
  _Bool b : 1;
} s;

int main(void)
{
  s.b = 1;
  int old = s.b++;
  assert(old == 1);
  assert(s.b == 0);
  return 0;
}
// Imported from ESBMC regression/cstd/github_9352_fail
// RUN: --std c11
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^Properties: \d+ checked, \d+ passed, 1 failed, \d+ not checked

#include <assert.h>

struct pair
{
  int *first;
};

union scalar_or_pair
{
  struct pair p;
  int *raw;
};

int x = -2;
int y = 7;

int main()
{
  union scalar_or_pair u;
  u.p.first = &y;
  assert(*u.raw == -2);
  return 0;
}
// Imported from ESBMC regression/esbmc/union_pun_pointer_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$

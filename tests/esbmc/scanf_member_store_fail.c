#include <assert.h>
#include <stdio.h>

struct s
{
  int a;
  int b;
} g = {1, 2};

int main()
{
  int r = sscanf("5", "%d", &g.b);
  if (r == 1)
    assert(g.b == 2);
  return 0;
}
// Imported from ESBMC regression/esbmc/scanf_member_store_fail
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL

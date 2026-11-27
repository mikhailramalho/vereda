#include <assert.h>
int main()
{
  assert(((int){0} = 42) == 42);
}
// Imported from ESBMC regression/esbmc/github_591-1
// EXPECT: ^VERIFICATION SUCCESSFUL$

#include <assert.h>
int main()
{
  do
  {
  } while (0+0);
  assert(0);
}
// Imported from ESBMC regression/esbmc/github_2602
// EXPECT: ^VERIFICATION FAILED$

#include <assert.h>
int main()
{
  do
  {
  } while (1);
  assert(0);
}
// Imported from ESBMC regression/esbmc/github_2602_1
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: over 300 s in CI
// MODE: KNOWNBUG
// Vereda: KNOWNBUG test passed unexpectedly; promote it to CORE
// MODE: CORE

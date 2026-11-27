#include <assert.h>

int main(void)
{
  int x;
  if (x > 0)
    x--;
  else
    x++;
  assert(x != 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_4309-cap
// Needs: --all-witnesses --max-witnesses 1
// RUN: --all-witnesses --max-witnesses 1 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: Witness 1 of 1
// EXPECT: --max-witnesses cap reached
// EXPECT: ^  NOTE: --max-witnesses cap reached; more witnesses may exist\.$
// Vereda: verdict none

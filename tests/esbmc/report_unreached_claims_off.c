#include <assert.h>
int main(void)
{
  int x = 0;
  if (x)
    assert(0);
  assert(x == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/report_unreached_claims_off
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: (?s)\A(?!.*never reached)

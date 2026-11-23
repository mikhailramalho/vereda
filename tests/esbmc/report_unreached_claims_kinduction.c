#include <assert.h>
void never_called(int v)
{
  assert(v > 0);
}
int main(void)
{
  int i = 0;
  while (i < 3)
    i++;
  assert(i == 3);
  return 0;
}
// Imported from ESBMC regression/esbmc/report_unreached_claims_kinduction
// Needs: --report-unreached-claims --k-induction
// RUN: --report-unreached-claims --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

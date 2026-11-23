#include <assert.h>

/* C23 6.3.1.2 still normalises to 0 or 1, so this must be reported. */
int main(void)
{
  bool n = (bool)7;
  assert(n != true);

  return 0;
}
// Imported from ESBMC regression/esbmc/c23_bool_literal_fail
// Needs: --std c23
// RUN: --std c23
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none

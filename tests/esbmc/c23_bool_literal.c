#include <assert.h>

/* C23 6.4.4.5 predefines bool, true and false, so no header is included. */
int main(void)
{
  bool t = true, f = false;
  assert(t);
  assert(!f);
  assert(t != f);
  assert((int)true == 1);
  assert((int)false == 0);

  bool n = (bool)7;
  assert(n == true);

  return 0;
}
// Imported from ESBMC regression/esbmc/c23_bool_literal
// RUN: --std c23 --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$

#include <assert.h>
#include "h.h"
int from_a(void);
int main(void)
{
  int x = from_a();
  int y = counter();
  assert(y == 3);
  return 0;
}
// Imported from ESBMC regression/esbmc/header_static_per_tu_fail
// RUN: --no-incremental-bmc
// FILES: a.c
// EXPECT: ^VERIFICATION FAILED$

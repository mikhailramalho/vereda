#include <string.h>

int main()
{
  char buf[4] = {'a', 'b', 'c', 'd'};
  return strnlen(buf, 5);
}
// Imported from ESBMC regression/esbmc/strnlen_model_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  dereference failure: array bounds violated$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL

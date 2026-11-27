#include <assert.h>

int main()
{
  assert(__builtin_abs(-3) == 3);
  assert(__builtin_labs(-3l) == -3);
  return 0;
}
// Imported from ESBMC regression/esbmc/builtin_abs_fail
// RUN: --multi-property --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: FAILED\s+\[main.assertion.2\].*__builtin_labs
// Vereda: unmet EXPECT: FAILED\s+\[main.assertion.2\].*__builtin_labs
// MODE: FUTURE
// Vereda: verdict FAILED

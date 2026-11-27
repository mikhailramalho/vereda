#include <assert.h>

void leaf(int value)
{
  assert(value == 7);
}

int main(void)
{
  leaf(7);
  return 0;
}
// Imported from ESBMC regression/esbmc/function_call_trace_pass
// Needs: --show-funccall-trace
// RUN: --show-funccall-trace --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

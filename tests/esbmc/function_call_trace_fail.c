#include <assert.h>

void leaf(int value)
{
  assert(value == 8);
}

int main(void)
{
  leaf(7);
  return 0;
}
// Imported from ESBMC regression/esbmc/function_call_trace_fail
// Needs: --show-funccall-trace
// RUN: --show-funccall-trace
// MODE: FUTURE
// EXPECT: Function call trace:
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none

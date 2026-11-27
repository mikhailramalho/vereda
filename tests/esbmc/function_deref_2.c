#include <assert.h>
void g(int x, int y)
{
  assert(0);
}
void trampo(void (*g)())
{
  g(5);
}
int main()
{
  trampo(g);
}
// Imported from ESBMC regression/esbmc/function_deref_2
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL

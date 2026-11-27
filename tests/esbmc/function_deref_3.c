#include <assert.h>

void g(int x)
{
  assert(x == 5);
}

void trampo(void (*fp)())
{
  fp(5);
}

int main()
{
  trampo(g);
}
// Imported from ESBMC regression/esbmc/function_deref_3
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$

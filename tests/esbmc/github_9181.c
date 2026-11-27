// https://github.com/esbmc/esbmc/issues/9181
#include <assert.h>

void h(void) {}

int main(void)
{
  assert(!__builtin_constant_p(h()));
  return __builtin_constant_p(h());
}
// Imported from ESBMC regression/esbmc/github_9181
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$

#include <assert.h>
#include <string.h>
int main()
{
  int z;
  int *p = &(int){0};
  memcpy(p, &z, sizeof(z));
  assert(*p == z);
}
// Imported from ESBMC regression/esbmc/github_591-2
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$

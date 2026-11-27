#include <assert.h>
#include <string.h>

int *p = &(int){5};

int main()
{
  assert(p);
  assert(*p == 5);
  ++*p;
  assert(*p == 6);
}
// Imported from ESBMC regression/esbmc/github_591-file-scope
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$

#include <assert.h>
#include <string.h>
int main()
{
  char b[4];
  char *e = stpcpy(b, "ab");
  assert(e == b + 2 && b[1] == 'b' && b[2] == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_11021_stpcpy_ok
// RUN: --solver z3 --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$

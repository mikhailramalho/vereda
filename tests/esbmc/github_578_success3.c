#include <stdint.h>
#include <assert.h>
int main()
{
  struct {
    unsigned x : 22;
  } s = { 2 };
  s.x <<= 20;
  assert(s.x == 2 << 20);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_578_success3
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$

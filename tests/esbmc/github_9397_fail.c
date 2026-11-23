#include <assert.h>
#include <stdio.h>

int main(void)
{
  char b[64];
  int r = snprintf(b, sizeof b, "%e|%g", 1.5, 0.0001);
  assert(r != 19);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9397_fail
// RUN: -m64
// EXPECT: ^VERIFICATION FAILED$

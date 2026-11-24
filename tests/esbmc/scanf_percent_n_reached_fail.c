#include <assert.h>
#include <stdio.h>

int main()
{
  int x, n = -1;
  int r = sscanf("12", "%d%n", &x, &n);
  if (r < 1)
    assert(n == -1);
  if (r == 1)
    assert(n == -1);
  return 0;
}
// Imported from ESBMC regression/esbmc/scanf_percent_n_reached_fail
// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: FAILED\s+\[main.assertion.2\]

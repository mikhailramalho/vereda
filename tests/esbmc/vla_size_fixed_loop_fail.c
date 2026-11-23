#include <assert.h>
int main(void)
{
  int n = 1;
  for (int k = 1; k <= 3; k++)
  {
    int a[n];
    n = n + 1;
    assert(sizeof(a) == n * sizeof(int));
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/vla_size_fixed_loop_fail
// RUN: --unwind 4
// EXPECT: ^VERIFICATION FAILED$

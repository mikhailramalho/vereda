#include <assert.h>

void compute_sum(void)
{
  int sum = 0;
  for (int i = 0; i < 10; i++)
    sum += i;
  assert(sum == 45);
}

int main(void)
{
  compute_sum();
  return 0;
}
// Imported from ESBMC regression/esbmc/unwindsetname_09_usr_effective
// RUN: --unwind 3 --unwindsetname c:@F@compute_sum#:0:11
// EXPECT: ^VERIFICATION SUCCESSFUL$

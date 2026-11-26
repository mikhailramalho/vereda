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
// Imported from ESBMC regression/esbmc/unwindsetname_08_lowers_global
// RUN: --unwind 20 --unwindsetname compute_sum:0:3
// EXPECT: ^  unwinding assertion loop \d+$
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
// Needs: --unwindsetname compute_sum:0:3

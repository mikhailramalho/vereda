/* Null restrict pointers designate no object and cannot violate the contract;
   the non-null guard must keep this from being flagged. */
#include <stddef.h>

void f(int *restrict a, int *restrict b)
{
  if (a && b)
  {
    *a = 1;
    *b = 2;
  }
}

int main(void)
{
  f(NULL, NULL);
  return 0;
}
// Imported from ESBMC regression/esbmc/restrict_null_param
// Needs: --restrict-check
// RUN: --restrict-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

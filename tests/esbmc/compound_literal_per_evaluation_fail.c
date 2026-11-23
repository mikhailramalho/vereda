#include <assert.h>

int main(void)
{
  int i = 5, hit = 0;
  for (i = 0; i < 3; i++)
    hit += (int[]){i}[0] == 2;
  assert(hit == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/compound_literal_per_evaluation_fail
// EXPECT: ^VERIFICATION FAILED$

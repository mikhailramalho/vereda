#include <assert.h>
#include <string.h>
int main(void)
{
  char d[4];
  memcpy(d, 0, 0);
  for (unsigned x = 0; x < 10; x++)
    assert(x < 10);
  return 0;
}
// Imported from ESBMC regression/k-induction/github_10856_proved
// Needs: --k-induction
// RUN: --k-induction --max-k-step 10 --no-pointer-check
// MODE: FUTURE
// EXPECT: ^Solution found by the inductive step \(k = 2\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

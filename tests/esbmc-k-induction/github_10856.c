#include <assert.h>
#include <string.h>
int main(void)
{
  char d[4];
  memcpy(d, 0, 0);
  for (unsigned x = 0; x < 10; x++)
    assert(x < 5);
  return 0;
}
// Imported from ESBMC regression/k-induction/github_10856
// Needs: --k-induction
// RUN: --k-induction --max-k-step 10 --no-pointer-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none

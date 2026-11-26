#include <string.h>

int main(void)
{
  int a[4] = {1, 2, 3, 4};
  memcpy(a - 1, a + 1, sizeof(int));
  return 0;
}
// Imported from ESBMC regression/esbmc/github_2307_negative_offset_fail
// EXPECT: ^  dereference failure: array bounds violated$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^  dereference failure: array bounds violated$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL

#include <assert.h>

int main()
{
  int x = 3;
  assert(x > 100);
  assert(x != 3);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7256
// RUN:
// MODE: FUTURE
// EXPECT: ^Properties: 2 checked, \d+ passed, 1 failed, \d+ not checked, 1 not checked$
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \A(?![\s\S]*produced no model)[\s\S]*\Z
// Vereda: verdict FAILED

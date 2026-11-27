#include <assert.h>

int main()
{
  int x = 3;
  assert(x > 100);
  assert(x != 3);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7256
// EXPECT: ^Properties: 1 checked, \d+ passed, 1 failed, 1 not checked$
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \A(?![\s\S]*produced no model)[\s\S]*\Z
// Vereda: unmet EXPECT: ^Properties: 1 checked, \d+ passed, 1 failed, 1 not checked$
// MODE: FUTURE
// Vereda: verdict FAILED

#include <string.h>
#include <assert.h>

typedef _Bool bool;

int main()
{
  bool v = __VERIFIER_nondet_bool();
  memset(&v, 0, sizeof(v) + 2);
}
// Imported from ESBMC regression/esbmc/github_1458-false
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL

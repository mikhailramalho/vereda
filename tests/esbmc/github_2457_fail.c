#include <assert.h>

int main()
{
  char a[4];
  assert(__CPROVER_rw_ok(a, 8));
  return 0;
}
// Imported from ESBMC regression/esbmc/github_2457_fail
// Needs: __CPROVER_rw_ok
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$

#include <time.h>
#include <assert.h>

int main()
{
  time_t a = time(NULL);
  time_t b = time(NULL);
  double d1 = difftime(a, b);
  double d2 = difftime(b, a);
  assert(d1 == -d2);
  return 0;
}
// Imported from ESBMC regression/esbmc/time_h_difftime_antisymmetry
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED

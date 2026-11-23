#include <time.h>
#include <assert.h>

int main()
{
  time_t original = 1000000;
  struct tm *broken = localtime(&original);
  if (broken != (void *)0)
  {
    time_t reconstructed = mktime(broken);
    assert(difftime(original, reconstructed) == 0.0);
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/time_h_mktime_localtime_roundtrip
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED

#include <time.h>

int main()
{
  clock_t c = clock();
  (void)c;
  return 0;
}
// Imported from ESBMC regression/esbmc/time_h_clock_callable
// EXPECT: ^VERIFICATION SUCCESSFUL$

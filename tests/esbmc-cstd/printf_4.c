#include <stdio.h>
#include <assert.h>

int main()
{
  char ss[] = "runoob";

  int x = printf("%s", ss);
#ifndef _WIN32
  assert(x == 6);
  assert(++x == 7);
#endif
}
// Imported from ESBMC regression/cstd/printf_4
// Needs: --k-induction
// RUN: --32 --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

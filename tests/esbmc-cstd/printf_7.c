#include <stdio.h>
#include <assert.h>

int main()
{
  char ss[] = "runoob";
  char* fmt = "%s";

  int x = printf(fmt, ss);
#ifndef _WIN32
  assert(x == 6);
  assert(++x == 7);
#endif
}
// Imported from ESBMC regression/cstd/printf_7
// Needs: --k-induction
// RUN: -m32 --k-induction
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none

#include <stdio.h>
#include <assert.h>

int main()
{
  char *s = "abcde1234151";
  int data = 1000000;
  int x = printf("%s%d\n", s, data);
#ifndef _WIN32
  assert(x == 20);
#endif
  x += 1;
}
// Imported from ESBMC regression/cstd/printf_3
// RUN: -m32
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict FAILED

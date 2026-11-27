#include <stdio.h>
#include <assert.h>
int main()
{
  char *s = "abcde123415";
  int x = printf("%s\n", s);
  // no body for function __stdio_common_vfprintf
#ifndef _WIN32
  assert(x == 12);
  x+=1;
#endif
  
}
// Imported from ESBMC regression/cstd/printf
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check -m32 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict FAILED

#include <stdio.h>
#include <assert.h>

int main()
{
  char *s = "abcde1234151";
  int data = 1000000;
  int x = printf("%s%d\n", s, data);
  assert(x != 20);
}
// Imported from ESBMC regression/cstd/printf_bug_4
// RUN: -m32 --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$

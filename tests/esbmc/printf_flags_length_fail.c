#include <assert.h>
#include <stdio.h>

int main(void)
{
  int n = printf("%+d", 5);
  assert(n == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/printf_flags_length_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  file .*printf_flags_length_fail\.c line 7 column 3 function main$

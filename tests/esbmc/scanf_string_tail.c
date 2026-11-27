#include <assert.h>
#include <stdio.h>

int main()
{
  char buf[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
  sscanf("xy", "%s", &buf[2]);
  assert(buf[0] == 'a');
  assert(buf[1] == 'b');
  return 0;
}
// Imported from ESBMC regression/esbmc/scanf_string_tail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$

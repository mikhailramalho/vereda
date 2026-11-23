#include <assert.h>
#include <stdio.h>

int main()
{
  char buf[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
  int r = sscanf("xy", "%s", &buf[2]);
  if (r == 1)
    assert(buf[3] == 'd');
  return 0;
}
// Imported from ESBMC regression/esbmc/scanf_string_tail_fail
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL

#include <assert.h>
#include <stdio.h>

int main()
{
  char buf[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
  int r = sscanf("x", "%s", &buf[8]);
  if (r == 1)
    assert(buf[0] == 'a');
  return 0;
}
// Imported from ESBMC regression/esbmc/scanf_tail_at_end_fail
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL

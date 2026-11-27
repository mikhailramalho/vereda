#include <assert.h>
#include <stdio.h>

int main()
{
  char buf[8] = {0};
  char fmt[3] = {'%', 's', 0};
  int r = sscanf("x", fmt, buf);
  if (r == 1)
    assert(buf[5] == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/scanf_nonliteral_format_fail
// EXPECT: ^VERIFICATION FAILED$

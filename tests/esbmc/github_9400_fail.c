/* #9400: %ls of L"ab" writes two characters. */
#include <assert.h>
#include <stdio.h>
#include <wchar.h>

int main(void)
{
  char b[64];
  int r = snprintf(b, sizeof b, "%ls", L"ab");
  assert(r != 2);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9400_fail
// RUN: -m64
// EXPECT: ^VERIFICATION FAILED$

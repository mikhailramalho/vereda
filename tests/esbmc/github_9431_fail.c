#include <assert.h>
#include <stdio.h>
int main(void)
{
  int n = -1;
  printf("ab%n", &n);
  assert(n != 2);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9431_fail
// EXPECT: ^VERIFICATION FAILED$

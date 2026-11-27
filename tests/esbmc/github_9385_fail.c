// #9385: fgets can return NULL at end-of-file.
#include <assert.h>
#include <stdio.h>

int main(void)
{
  char buf[16];
  char *r = fgets(buf, sizeof buf, stdin);
  assert(r != NULL);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9385_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$

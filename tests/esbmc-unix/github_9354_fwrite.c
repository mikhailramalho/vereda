/* #9354: fwrite of exactly the buffer, element-wise and as one block. */
#include <stdio.h>
#include <string.h>

int main(void)
{
  int reply[4];
  memset(reply, 0, sizeof reply);
  volatile size_t len = 4;
  fwrite(reply, sizeof(int), len, stdout);
  fwrite(reply, sizeof reply, 1, stdout);
  fwrite(reply + 4, 1, 0, stdout);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_9354_fwrite
// RUN: -m64 --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$

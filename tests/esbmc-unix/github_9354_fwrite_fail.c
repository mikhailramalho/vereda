/* #9354: fwrite reads size * nitems bytes from its buffer. */
#include <stdio.h>
#include <string.h>

int main(void)
{
  char reply[4];
  memcpy(reply, "pong", 4);
  volatile size_t len = 64;
  fwrite(reply, 1, len, stdout);
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_9354_fwrite_fail
// RUN: -m64
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: FAILED +\[fwrite\.array-bounds-violated\.[0-9]+\].*dereference failure: array bounds violated
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL

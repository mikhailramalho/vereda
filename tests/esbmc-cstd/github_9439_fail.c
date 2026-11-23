#include <string.h>

int main(void)
{
  char name[4] = "abc";
  return memchr(name, 'z', 64) != NULL;
}
// Imported from ESBMC regression/cstd/github_9439_fail
// RUN: -m64 --unwind 20
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: dereference failure on memchr: reading memory segment of size 4 with 64 bytes$
// Vereda: unmet EXPECT: dereference failure on memchr: reading memory segment of size 4 with 64 bytes$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL

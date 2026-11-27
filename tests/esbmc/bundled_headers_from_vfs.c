#include <string.h>

int main(void)
{
  char b[4];
  strcpy(b, "abc");
  __VERIFIER_assert(b[0] == 'a'); // strcpy copied the first byte
  return 0;
}
// Imported from ESBMC regression/esbmc/bundled_headers_from_vfs
// MODE: FUTURE
// EXPECT: ^clang invocation:.*'-isystem' '([A-Za-z]:)?/esbmc-vfs/libc/headers'
// EXPECT: ^clang invocation:.*'-resource-dir' '([A-Za-z]:)?/esbmc-vfs/clang'
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict SUCCESSFUL

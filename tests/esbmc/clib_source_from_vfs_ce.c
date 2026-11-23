#include <string.h>

int main()
{
  char a[4];
  char b[4];
  memcpy(a, b, 8);
  return a[0];
}
// Imported from ESBMC regression/esbmc/clib_source_from_vfs_ce
// RUN: --incremental-bmc
// EXPECT: file ([A-Za-z]:)?/esbmc-vfs/libc/library/string\.c line \d+ column \d+ function memcpy
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: file ([A-Za-z]:)?/esbmc-vfs/libc/library/string\.c line \d+ column \d+ function memcpy
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL

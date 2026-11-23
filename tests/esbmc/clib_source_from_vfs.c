#include <math.h>

int main()
{
  float f = 1.0f;
  return cosf(f) > 0.0f ? 0 : 1;
}
// Imported from ESBMC regression/esbmc/clib_source_from_vfs
// Needs: --goto-functions-only REQUIRES bundled_libm
// RUN: --goto-functions-only
// MODE: FUTURE
// EXPECT: file ([A-Za-z]:)?/esbmc-vfs/libc/library/libm/musl/__cosdf\.c line \d+
// EXPECT: file ([A-Za-z]:)?/esbmc-vfs/libc/library/libm/pow\.c line \d+
// Vereda: verdict none

#include <assert.h>

#ifndef _WIN32 // As of now, LLVM for Windows don't support this
static_assert(sizeof(long) != 2, "Code relies on int being exactly 4 bytes");
#endif
int main(void)
{
  #ifndef _WIN32
  static_assert(sizeof(long) != 2, "Code relies on int being exactly 4 bytes");
  #endif
  return 0;
}
// Imported from ESBMC regression/llvm/static_assert_fail
// Needs: REQUIRES arch32
// RUN: --32
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

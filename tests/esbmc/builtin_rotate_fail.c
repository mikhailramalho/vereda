#include <assert.h>

int main()
{
  assert(__builtin_rotateleft32(0x80000001u, 1) == 3);
  assert(__builtin_rotateright32(0x80000001u, 1) == 3);
  return 0;
}
// Imported from ESBMC regression/esbmc/builtin_rotate_fail
// RUN: --multi-property --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: FAILED\s+\[main.assertion.2\].*__builtin_rotateright32
// Vereda: unmet EXPECT: FAILED\s+\[main.assertion.2\].*__builtin_rotateright32
// MODE: FUTURE
// Vereda: verdict FAILED

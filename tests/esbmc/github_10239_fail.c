/* argv[3] and argv[4] are separate strings that may differ, and reading past
   the end of one is still caught (esbmc/esbmc#10239). */
#include <assert.h>
int main(int argc, char **argv)
{
  if (argc == 5)
  {
    assert(argv[3][0] == argv[4][0]);
    return argv[4][256];
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10239_fail
// RUN: --multi-property --no-incremental-bmc
// EXPECT: ^\s*PASSED\s+\[main\.null-pointer-dereference\.\d+\]
// EXPECT: ^\s*FAILED\s+\[main\.array-bounds-violated\.\d+\]\s+line 9\s
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^\s*PASSED\s+\[main\.null-pointer-dereference\.\d+\]
// MODE: FUTURE
// Vereda: verdict FAILED

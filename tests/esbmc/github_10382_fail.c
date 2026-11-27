/* envp[0] and envp[1] are separate strings that may differ, reading past the
   end of one is still caught, and envp[0] is NULL for an empty environment
   (esbmc/esbmc#10382). */
#include <assert.h>
int main(int argc, char **argv, char **envp)
{
  if (envp[0] && envp[1])
  {
    assert(envp[0][0] == envp[1][0]);
    return envp[1][256];
  }
  return envp[0][0];
}
// Imported from ESBMC regression/esbmc/github_10382_fail
// RUN: --multi-property
// EXPECT: ^\s*PASSED\s+\[main\.null-pointer-dereference\.\d+\]\s+line\s+9\s
// EXPECT: ^\s*FAILED\s+\[main\.array-bounds-violated\.\d+\]\s+line\s+10\s
// EXPECT: ^\s*FAILED\s+\[main\.null-pointer-dereference\.\d+\]\s+line\s+12\s
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^\s*PASSED\s+\[main\.null-pointer-dereference\.\d+\]\s+line\s+9\s
// MODE: FUTURE
// Vereda: verdict FAILED

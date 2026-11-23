#include <assert.h>
#include <stdio.h>

int main()
{
  /* %02X of 0xAB = "AB" (2 chars), so r must equal 2 */
  int r = printf("%02X", (unsigned)0xAB);
  assert(r == 2);
}
// Imported from ESBMC regression/esbmc/github_1174_pass
// RUN: --unwind 1
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED

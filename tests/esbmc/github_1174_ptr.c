#include <assert.h>
#include <stdio.h>

int main()
{
  int x = 0;
  // %p outputs "0x" + pointer_width/4 hex digits. On 64-bit that is 18 chars;
  // on 32-bit that is 10 chars. Require at least 10 either way, which is
  // strictly larger than the "0" (length 1) the previous model emitted.
  int r = printf("%p", (void *)&x);
  assert(r >= 10);
}
// Imported from ESBMC regression/esbmc/github_1174_ptr
// RUN: --unwind 1
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED

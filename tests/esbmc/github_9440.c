#include <assert.h>
#include <stdio.h>

int main(void)
{
  char c = (char)0xe9;
  int r = putchar(c);
  assert(r == 233 || r == EOF);
  r = putchar(300);
  assert(r == 44 || r == EOF);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9440
// RUN: -m64 --unwind 20
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED

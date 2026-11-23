// #9385: fgets returns NULL and leaves the array unchanged at end-of-file
// before any character; otherwise it returns str with a terminator in bounds.
#include <assert.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
  char buf[4] = {'x', 'x', 'x', 'x'};
  char *r = fgets(buf, sizeof buf, stdin);
  if (r == NULL)
    assert(memcmp(buf, "xxxx", 4) == 0);
  else
    assert(r == buf && memchr(buf, '\0', 4) != NULL);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9385
// RUN: --unwind 5
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED

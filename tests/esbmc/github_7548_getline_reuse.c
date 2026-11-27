/* github #7548: the caller-supplied-buffer shape. getline may replace the
 * object, so what holds afterwards is about *lineptr and *n, not the pointer
 * the caller passed in. */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(void)
{
  char *line = malloc(100);
  size_t cap = 100;
  if (line == NULL)
    return 0;

  ssize_t len = getline(&line, &cap, stdin);
  if (len >= 0)
  {
    assert(cap > (size_t)len);
    assert(line[len] == '\0');
  }

  free(line);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7548_getline_reuse
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: FUTURE test passed unexpectedly; promote it to CORE
// MODE: CORE

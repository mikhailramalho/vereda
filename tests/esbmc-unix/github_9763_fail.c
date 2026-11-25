// GitHub #9763: a direct printf next to a v* call is still checked.
#include <stdarg.h>
#include <stdio.h>

static void say(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);
}

int main(void)
{
  say("%s %d\n", "abc", 3);
  printf("%s %d\n", "abc");
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/github_9763_fail
// Needs: --printf-check
// RUN: --printf-check --32
// MODE: FUTURE
// EXPECT: ^  FAILED       \[main\.assertion\.[0-9]+\]  line 16  printf has more format specifiers than arguments$
// EXPECT: ^Properties: [0-9]+ checked, \d+ passed, 1 failed, \d+ not checked
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none

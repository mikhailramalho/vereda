// #9895: %s reads a buffer whose lifetime has ended.
#include <stdio.h>

int main()
{
  char *data;
  {
    char buf[4] = "abc";
    data = buf;
  }
  printf("%s\n", data);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9895_printf_expired
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: dereference failure: accessed expired variable pointer `buf'
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL

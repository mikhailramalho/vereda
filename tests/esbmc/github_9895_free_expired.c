// #9895: freeing a dead stack buffer is a bad free, not a dereference.
#include <stdlib.h>

int main()
{
  char *data;
  {
    char buf[4];
    data = buf;
  }
  free(data);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9895_free_expired
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: dereference failure: free\(\) of non-dynamic memory
// EXPECT: \A(?![\s\S]*accessed expired variable pointer)[\s\S]*\Z
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL

#include <assert.h>

void inner(int x)
{
  assert(x != 2);
}

void middle(int x)
{
  inner(x + 1);
}

int main()
{
  middle(1);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_1233
// Needs: --show-stacktrace
// RUN: --show-stacktrace
// MODE: FUTURE
// EXPECT: ^Stack trace:$
// EXPECT: ^  inner at file .*github_1233\.c line 10 column 3 function middle$
// EXPECT: ^  middle at file .*github_1233\.c line 15 column 3 function main$
// EXPECT: ^  main at file .*github_1233\.c line 13 column 1$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none

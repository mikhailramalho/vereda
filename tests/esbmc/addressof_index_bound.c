// A pointer loop whose exit is spelled &a[4] while the induction variable is
// built by increments from a. The two forms must fold to the same offset for
// the guard to be decided syntactically; without the fold the same program
// with no --unwind runs forever.
#include <assert.h>

int main(void)
{
  int a[4] = {1, 1, 1, 1};
  int sum = 0;

  for (int *p = a; p != &a[4]; ++p)
    sum += *p;

  assert(sum == 4);
  return 0;
}
// Imported from ESBMC regression/esbmc/addressof_index_bound
// RUN: --unwind 6
// EXPECT: ^VERIFICATION SUCCESSFUL$

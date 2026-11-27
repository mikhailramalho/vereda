#include <assert.h>
#include <string.h>

/* memset over a whole array of pointers: every element becomes NULL. */
int x;

int main()
{
  int *a[4] = {&x, &x, &x, &x};
  memset(a, 0, sizeof a);
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 4);
  assert(a[i] == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/memset_whole_pointer_array
// EXPECT: ^VERIFICATION SUCCESSFUL$

// scandir reads entries, passes each to the filter, and stores the ones it
// keeps, each a new object, in a new array sorted with compar, so free
// takes each entry and then the array (#503).
// RUN: --unwind 3 --no-unwinding-assertions
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <assert.h>
#include <dirent.h>
#include <stdlib.h>

static int seen, compared;

static int keep(const struct dirent *d) {
  ++seen;
  return d->d_name[0] != '.';
}

static int by_inode(const struct dirent **a, const struct dirent **b) {
  ++compared;
  return (*a)->d_ino < (*b)->d_ino ? -1 : (*a)->d_ino > (*b)->d_ino;
}

int main(void) {
  struct dirent **v;
  int n = scandir(".", &v, keep, by_inode);
  if (n < 0)
    return 0;
  assert(n <= seen);
  assert(n < 2 || compared > 0);
  for (int i = 0; i < n; ++i) {
    assert(v[i]->d_name[0] != '.');
    free(v[i]);
  }
  free(v);
  return 0;
}

// #7907: memset through the address of a vector writes every lane.
#include <assert.h>
#include <string.h>

typedef int v4i __attribute__((__vector_size__(16)));

int main(void)
{
  v4i v = {1, 2, 3, 4};
  memset(&v, 1, sizeof v);
  assert(v[2] == 3);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7907_memset_fail
// EXPECT: ^VERIFICATION FAILED$

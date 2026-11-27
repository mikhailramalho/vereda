#include <stdlib.h>
#include <assert.h>

int main()
{
  void *b = malloc(4);
  assert(0);
}
// Imported from ESBMC regression/esbmc/no_simplify_no_slice_malloc
// EXPECT: ^VERIFICATION FAILED$

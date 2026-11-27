#include <stddef.h>
#include <assert.h>
void *alloca(size_t size);
int main(void)
{
  int *p;
  {
    p = alloca(sizeof(int));
    *p = 1;
  }
  assert(*p == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8709_alloca_block_scope
// EXPECT: ^VERIFICATION SUCCESSFUL$

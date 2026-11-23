#include <assert.h>
#include <stdlib.h>

int order;

void first(void)
{
  order = order * 10 + 1;
  assert(order == 21);
}

void second(void)
{
  order = order * 10 + 2;
}

int main(void)
{
  atexit(first);
  atexit(second);
  return 0;
}
// Imported from ESBMC regression/esbmc/atexit_after_main_return
// EXPECT: ^VERIFICATION SUCCESSFUL$

/* A braced string literal initialises the whole char array (C11 6.7.9p14). */
#include <assert.h>
int main()
{
  char a[4] = {"ab"};
  assert(a[1] == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/string_literal_brace_init_fail
// EXPECT: ^VERIFICATION FAILED$

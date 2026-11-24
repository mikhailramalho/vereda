#include <string.h>
const int t[2] = {1, 2};
int main()
{
  int s[2] = {7, 8};
  memmove((void *)t, s, sizeof t);
  return 0;
}
// Imported from ESBMC regression/esbmc/const_object_write_memmove_fail
// EXPECT: ^  dereference failure: write access to const object `t'$
// EXPECT: ^VERIFICATION FAILED$

#include <assert.h>
struct S
{
  unsigned int x, y;
};
struct S s[2];
int main(void)
{
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 2);
  unsigned char *p = (unsigned char *)&s[i];
  *(unsigned long long *)p = 0x0000000200000001ULL;
  assert(s[i].x == 1);
  assert(s[i].y == 2);
  return 0;
}
// Imported from ESBMC regression/esbmc/struct_dyn_offset_spanning_flat
// EXPECT: ^VERIFICATION SUCCESSFUL$

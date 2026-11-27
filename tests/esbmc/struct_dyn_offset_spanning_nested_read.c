#include <assert.h>
struct T
{
  unsigned short a, b, c;
};
struct S
{
  struct T t;
  unsigned short d;
};
struct S s[2] = {{{1, 2, 0x0201}, 0x0403}, {{1, 2, 0x0201}, 0x0403}};
int main(void)
{
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 2);
  unsigned char *p = (unsigned char *)&s[i] + 4;
  assert(*(unsigned int *)p == 0x04030201u);
  return 0;
}
// Imported from ESBMC regression/esbmc/struct_dyn_offset_spanning_nested_read
// EXPECT: ^VERIFICATION SUCCESSFUL$

#include <assert.h>
union UF { unsigned int u; };
int main(void)
{
  float f = 1.0f;
  union UF v = *(union UF *)&f;
  assert(v.u == 0x3f800000u);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10939_float_union
// EXPECT: ^VERIFICATION SUCCESSFUL$

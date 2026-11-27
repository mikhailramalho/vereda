#include <assert.h>
struct SF { unsigned int u; };
int main(void)
{
  float f = 1.0f;
  struct SF v = *(struct SF *)&f;
  assert(v.u == 0x3f800000u);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10939_struct_const
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$

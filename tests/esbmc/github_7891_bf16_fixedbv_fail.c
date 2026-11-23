// #7891: under --fixedbv, __bf16 has 8 fraction bits, so 2^-9 is lost.
// Pins the width and the fixedbv branch, not the bfloat16 format.
#include <assert.h>

int main(void)
{
  __bf16 small = (__bf16)0.001953125f;
  assert((float)small == 0.001953125f);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7891_bf16_fixedbv_fail
// Needs: --fixedbv
// RUN: --fixedbv
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none

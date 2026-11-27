// #7891: __bf16 arithmetic reaches the solver in bfloat16's format.
#include <assert.h>

__bf16 __VERIFIER_nondet_bf16(void);

int main(void)
{
  __bf16 x = __VERIFIER_nondet_bf16();
  __VERIFIER_assume((float)x == 128.0f);
  // 128.5 needs 9 significand bits: it ties to even in bfloat16's 8.
  __bf16 y = x + (__bf16)0.5f;
  assert((float)y == 128.5f);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7891_bf16_nondet_fail
// RUN: --fp2bv --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$

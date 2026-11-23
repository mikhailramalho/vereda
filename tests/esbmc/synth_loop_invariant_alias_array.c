/* Alias via array element write onto the accumulator. */
#include <stdint.h>
#include <assert.h>
int main(void){ uint32_t n; __VERIFIER_assume(n>=1&&n<=3);
  uint64_t arr[2]; arr[0] = 0; arr[1] = 0;
  uint64_t *p = &arr[0]; p[0] = 5;
  uint64_t s = 0; uint64_t *w = &s; w[0] = 9;
  uint64_t i = 1;
  while (i<=n) { s = s + 2; i++; }
  assert(s == 9 + (uint64_t)n*2); return 0; }
// Imported from ESBMC regression/esbmc/synth_loop_invariant_alias_array
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants --unwind 10
// MODE: FUTURE
// EXPECT: \A(?![\s\S]*Synthesised loop invariants)[\s\S]*\Z
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

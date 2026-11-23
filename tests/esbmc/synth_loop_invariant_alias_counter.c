/* Alias write to the COUNTER's entry value before the loop. */
#include <stdint.h>
#include <assert.h>
int main(void){ uint32_t n; __VERIFIER_assume(n>=2&&n<=4);
  uint64_t i = 0; uint64_t *p = &i; *p = 1;
  uint64_t s = 0;
  while (i < n) { s = s + 1; i++; }
  assert(s == n - 1); return 0; }
// Imported from ESBMC regression/esbmc/synth_loop_invariant_alias_counter
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants --unwind 10
// MODE: FUTURE
// EXPECT: \A(?![\s\S]*Synthesised loop invariants)[\s\S]*\Z
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

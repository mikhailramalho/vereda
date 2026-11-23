/* Alias via struct field write. */
#include <stdint.h>
#include <assert.h>
struct S { uint64_t v; };
int main(void){ uint32_t n; __VERIFIER_assume(n>=1&&n<=3);
  uint64_t s = 0; struct S *q = (struct S*)&s; q->v = 5;
  uint64_t i = 1;
  while (i<=n) { s = s + 2; i++; }
  assert(s == 5 + (uint64_t)n*2); return 0; }
// Imported from ESBMC regression/esbmc/synth_loop_invariant_alias_field
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants --unwind 10
// MODE: FUTURE
// EXPECT: \A(?![\s\S]*Synthesised loop invariants)[\s\S]*\Z
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

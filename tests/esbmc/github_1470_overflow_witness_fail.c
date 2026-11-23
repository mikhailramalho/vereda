// Regression for github #1470: the GraphML violation witness for an arithmetic
// overflow must record the assignment to the overflowing variable, otherwise
// external validators (UAutomizer/CPAchecker) cannot replay the counterexample.
// Signed `data + 1` overflows iff data == INT64_MAX, so the witness must emit
// the assumption `data == 9223372036854775807` on the assignment edge.
#include <stdint.h>

extern int64_t __VERIFIER_nondet_int64(void);

int main(void)
{
  int64_t data = __VERIFIER_nondet_int64();
  int64_t result = data + 1; // overflow when data == INT64_MAX
  return result == 0;
}
// Imported from ESBMC regression/esbmc/github_1470_overflow_witness_fail
// Needs: --witness-output-graphml -
// RUN: --overflow-check --witness-output-graphml -
// MODE: FUTURE
// EXPECT: data == 9223372036854775807
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none

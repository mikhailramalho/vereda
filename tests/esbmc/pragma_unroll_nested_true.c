/* Test #pragma unroll N with nested loops
 * Outer loop has bound 10, pragma limits to 5
 * Inner loop has bound 8, pragma limits to 3
 * Both pragmas should be applied independently
 */

#include <stdint.h>

int main() {
  int sum = 0;

  #pragma unroll 5
  for (uint32_t i = 0; i < 10; i++) {

    #pragma unroll 3
    for (uint32_t j = 0; j < 8; j++) {
      sum += i + j;
    }
  }

  // Verification succeeds with limited unrolling
  return 0;
}
// Imported from ESBMC regression/esbmc/pragma_unroll_nested_true
// RUN: --no-unwinding-assertions --no-incremental-bmc
// MODE: FUTURE
// EXPECT: Applying #pragma unroll 3 to loop
// EXPECT: Applying #pragma unroll 5 to loop
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict SUCCESSFUL

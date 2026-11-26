/* Test #pragma unroll N with while loop
 * Loop bound is 10, but pragma limits unrolling to 5
 * Demonstrates pragma works with while loops
 */

int main() {
  int sum = 0;
  int i = 0;

  #pragma unroll 5
  while (i < 10) {
    sum += i;
    i++;
  }

  // Only iterations 0-4 are explored due to pragma
  return 0;
}
// Imported from ESBMC regression/esbmc/pragma_unroll_while_true
// RUN: --no-unwinding-assertions
// MODE: FUTURE
// EXPECT: Applying #pragma unroll 5 to loop
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict SUCCESSFUL

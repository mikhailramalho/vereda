/* Test #pragma unroll with undefined macro
 * Should fail with parsing error since UNDEFINED_MACRO is not defined
 */

int main() {
  int sum = 0;

  #pragma unroll UNDEFINED_MACRO
  for (int i = 0; i < 10; i++) {
    sum += i;
  }

  return 0;
}
// Imported from ESBMC regression/esbmc/pragma_unroll_undefined_macro_fail
// RUN: --no-unwinding-assertions --no-incremental-bmc
// EXPECT: PARSING ERROR
// Vereda: unmet EXPECT: PARSING ERROR
// MODE: FUTURE
// Vereda: verdict none

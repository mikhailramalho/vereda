/*
 * According with the section "Bitwise shift operators" of the C99 ISO specification:
 * "If the value of the right operand is negative or is greater than or equal to the
    width of the promoted left operand, the behavior is undefined."
 */

int main() {
  int n = 1 << -1; // right operand is negative
  return 0;
}
// Imported from ESBMC regression/esbmc/ub_01_shift_left_negative_op2
// RUN: --ub-signed-overflow-check --ub-shift-check --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$

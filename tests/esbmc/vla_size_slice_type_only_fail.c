/* The loop stores a 1 in row 0, so the sum is positive. */
long n, sum;
int row, col, done;
short idx, fixed_row;
void abort();
void assume(int cond) {
  if (!cond)
    abort();
}
void check(int cond) {
  if (!cond)
  ERROR: abort();
}
int __VERIFIER_nondet_int();
short __VERIFIER_nondet_short();
int main() {
  n = __VERIFIER_nondet_short();
  assume(n > 0);
  int array[n][n];
  for (; row < n; row++) {
    col = 0;
    for (; col < n; col++)
      array[row][col] = 0;
  }
  while (1) {
    idx = __VERIFIER_nondet_short();
    assume(idx >= 0 && idx < n);
    array[fixed_row][idx] = 1;
    done = __VERIFIER_nondet_int();
    if (done)
      break;
  }
  row = 0;
  for (; row < n; row++) {
    col = 0;
    for (; col < n; col++)
      sum = sum + array[row][col];
  }
  check(sum == 0);
}
// Imported from ESBMC regression/esbmc/vla_size_slice_type_only_fail
// Needs: --error-label ERROR
// RUN: --no-bounds-check --vla-size-check=false --error-label ERROR --unwind 3 --no-unwinding-assertions
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none

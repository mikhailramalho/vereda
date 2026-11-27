// The closed form counts in unsigned arithmetic, which the check would flag.
int a[100];
int main() {
  int j = -5;
  for (int i = 0; i < 100; i++) { a[i] = j; j += 1; }
  __VERIFIER_assert(j == 95);
}
// Imported from ESBMC regression/esbmc/array_loop_summary_overflow_check
// RUN: --ub-signed-overflow-check --unsigned-overflow-check --ub-shift-check --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$

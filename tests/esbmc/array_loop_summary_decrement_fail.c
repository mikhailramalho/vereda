// A counter decremented before the write reads it, wrapping below zero.
// RUN:
// RUN: --no-summarize-array-loops
#define N 100000
unsigned a[N];
unsigned __VERIFIER_nondet_uint(void);
int main() {
  unsigned j = 7;
  for (unsigned i = 0; i < N; i++) { j -= 2; a[i] = j; }
  unsigned x = __VERIFIER_nondet_uint();
  __VERIFIER_assume(x < N);
  __VERIFIER_assert(a[x] == 7 - 2 * x);
}
// Imported from ESBMC regression/esbmc/array_loop_summary_decrement_fail
// EXPECT: ^VERIFICATION FAILED$

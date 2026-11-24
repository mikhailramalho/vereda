// A second counter read by the write before its own update.
#define N 100000
int a[N];
unsigned __VERIFIER_nondet_uint(void);
int main() {
  int j = 0;
  for (int i = 0; i < N; i++) { a[i] = j; j += 3; }
  unsigned x = __VERIFIER_nondet_uint();
  __VERIFIER_assume(x < N);
  __VERIFIER_assert(a[x] == 3 * (int)x);
  __VERIFIER_assert(j == 3 * N);
}
// Imported from ESBMC regression/esbmc/array_loop_summary_counter
// EXPECT: ^VERIFICATION SUCCESSFUL$

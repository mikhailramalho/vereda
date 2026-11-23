// A counter stepping by 2 writes every other element.
#define N 100000
int a[N];
unsigned __VERIFIER_nondet_uint(void);
int main() {
  unsigned i;
  for (i = 1; i < N; i += 2) a[i] = 1;
  unsigned x = __VERIFIER_nondet_uint();
  __VERIFIER_assume(x < N);
  __VERIFIER_assert(a[x] == (x % 2));
  __VERIFIER_assert(i == N);
}
// Imported from ESBMC regression/esbmc/array_loop_summary_stride_fail
// EXPECT: ^VERIFICATION FAILED$
// Vereda: over 300 s
// MODE: KNOWNBUG

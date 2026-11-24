// hi widens an unsigned short, so i += 2 cannot wrap past it.
unsigned short __VERIFIER_nondet_ushort(void);
int main() {
  unsigned short N = __VERIFIER_nondet_ushort();
  unsigned x = 0;
  while (x < N) x += 2;
  __VERIFIER_assert(x % 2 == 0);
  __VERIFIER_assert(x == N);
}
// Imported from ESBMC regression/esbmc/array_loop_summary_stride_bound_fail
// EXPECT: ^VERIFICATION FAILED$

// The outer loop's body is only a loop that counts, so the outer loop is
// summarised as well, with its counter's last value, and neither loop is
// unrolled up to the bound (#504).
// RUN: --unwind 3
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned n = __VERIFIER_nondet_uint(), m = __VERIFIER_nondet_uint(), i, j;
  for (i = 0; i < n; i++)
    for (j = 0; j < m; j++)
      ;
  __VERIFIER_assert(i == n);
  return 0;
}

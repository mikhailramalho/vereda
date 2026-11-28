// The inner loop never ends where m is odd, and SCEV gives it no count, so
// the outer loop is not summarised past it: it unrolls, and the bound's
// unwinding check fails (#504).
// RUN: --unwind 3
// EXPECT: ^  FAILED +\[main\.assertion\.\d+\]  line \d+  unwinding assertion loop \d+$
// EXPECT: ^VERIFICATION FAILED$
extern unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  unsigned n = __VERIFIER_nondet_uint(), m = __VERIFIER_nondet_uint(), i, j;
  for (i = 0; i < n; i++)
    for (j = 0; j != m; j += 2)
      ;
  return 0;
}

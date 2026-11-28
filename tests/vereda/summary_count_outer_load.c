// The outer loop's body reads the inner loop's count from memory: the
// read has checks of its own (a[10] is out of bounds), so the outer loop
// is not summarised past it, unrolls, and the bound's unwinding check
// fails (#504).
// RUN: --unwind 3
// EXPECT: ^  FAILED +\[main\.\S+\]  line 12  unwinding assertion loop \d+$
// EXPECT: ^VERIFICATION FAILED$
extern unsigned __VERIFIER_nondet_uint(void);
int a[10];
int main(void) {
  unsigned n = __VERIFIER_nondet_uint(), i, j;
  for (i = 0; i < n; i++) {
    unsigned m = a[i];
    for (j = 0; j < m; j++)
      ;
  }
  return 0;
}

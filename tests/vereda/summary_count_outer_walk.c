// An outer loop with a check (a sanitizer trap, an assertion) or a store
// in its body has an iteration to walk, so it is not summarised around
// the loop inside it: it unrolls, and the bound's unwinding check fails
// on it (#504).
// RUN: --ub-signed-overflow-check --unwind 3 --multi-property
// EXPECT: ^  FAILED +\[main\.assertion\.\d+\]  line 17  unwinding assertion loop \d+$
// EXPECT: ^  FAILED +\[main\.assertion\.\d+\]  line 23  unwinding assertion loop \d+$
// EXPECT: ^  FAILED +\[main\.assertion\.\d+\]  line 29  unwinding assertion loop \d+$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);
static int a[1 << 20];

int main(void) {
  unsigned m = __VERIFIER_nondet_uint(), i, j;
  int s = 0;
  unsigned n = __VERIFIER_nondet_uint();
  for (i = 0; i < n; i++) {
    s = s + 1;
    for (j = 0; j < m; j++)
      ;
  }
  n = __VERIFIER_nondet_uint();
  for (i = 0; i < n; i++) {
    __VERIFIER_assert(i < n);
    for (j = 0; j < m; j++)
      ;
  }
  n = __VERIFIER_nondet_uint();
  for (i = 0; i < n; i++) {
    a[i] = 1;
    for (j = 0; j < m; j++)
      ;
  }
  return 0;
}

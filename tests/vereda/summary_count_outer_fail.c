// As summary_count_outer, but the assertion is wrong where the outer loop
// runs 5 times, past the bound: only the summary reaches that (#504).
// RUN: --unwind 3
// EXPECT: ^  FAILED +\[main\.assertion\.\d+\]  line 14  __VERIFIER_assert$
// EXPECT-NOT: unwinding assertion
// EXPECT: ^VERIFICATION FAILED$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned n = __VERIFIER_nondet_uint(), m = __VERIFIER_nondet_uint(), i, j;
  for (i = 0; i < n; i++)
    for (j = 0; j < m; j++)
      ;
  __VERIFIER_assert(i != 5);
  return 0;
}

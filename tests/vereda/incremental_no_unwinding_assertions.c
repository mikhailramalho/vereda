// As in ESBMC, --no-unwinding-assertions leaves incremental BMC on: each
// bound still decides from the unwinding assertions whether it is enough, and
// none is reported. The k-step flags go with it.
// RUN: --no-unwinding-assertions --no-summarize-array-loops --max-k-step 10
// RUN: --no-unwinding-assertions --no-summarize-array-loops --incremental-bmc
// EXPECT: ^\[PROGRESS\] Checking bound k = 4$
// EXPECT: FAILED +\[main\.assertion\.2\]
// EXPECT-NOT: \[main\.unwind
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int n = __VERIFIER_nondet_int(), s = 0;
  for (int i = 0; i < n; i++)
    s++;
  __VERIFIER_assert(s != 3);
  return 0;
}

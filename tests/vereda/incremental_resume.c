// Preserve calls, nondeterministic values, memory and the trace across bounds.
// RUN: --no-summarize-array-loops --incremental-bmc --max-k-step 20
// EXPECT: \[PROGRESS\] Resuming main
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int a[12], b[12];
__attribute__((noinline)) int value(void) {
  volatile int result = __VERIFIER_nondet_int();
  return result;
}
int main(void) {
  for (int i = 0; i < 4; ++i)
    a[i] = value() & 255;
  for (int i = 0; i < 12; ++i) {
    b[i] = a[i % 4] + i;
    __VERIFIER_assert(b[i] >= i);
  }
  for (int i = 0; i < 12; ++i)
    __VERIFIER_assert(b[i] == a[i % 4] + i);
  __VERIFIER_assert(b[11] >= 11);
}

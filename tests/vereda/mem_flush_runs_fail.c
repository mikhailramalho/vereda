// Failing sibling of mem_flush_runs: one element in a run differs and must be
// found by a symbolic read.
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned __VERIFIER_nondet_uint(void);

int main(void) {
  int a[32];
  for (int k = 0; k < 16; ++k)
    a[k] = 3;
  for (int k = 16; k < 32; ++k)
    a[k] = 7;
  a[20] = 8;

  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 32);
  __VERIFIER_assert(i < 16 ? a[i] == 3 : a[i] == 7);
  return 0;
}

// A late mismatch must not be guessed away from the quadratic prefix.
// RUN: --no-summarize-array-loops --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned char a[1024];
  for (unsigned i = 0; i < 1024; ++i)
    a[i] = (unsigned char)(7 + 3 * i + i * (i - 1) / 2);
  a[1023] += 1;
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k < 1024);
  unsigned value = a[k];
  if (k == 1023)
    __VERIFIER_assert(value == (unsigned char)(7 + 3 * k + k * (k - 1) / 2));
}

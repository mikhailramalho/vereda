// Fully written byte tables keep their modular values through compact flushing.
// RUN: --no-summarize-array-loops
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned char a[2048];
  for (unsigned i = 0; i < 2048; ++i)
    a[i] = (unsigned char)(250 + 7 * i);
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(k < 2048);
  __VERIFIER_assert(a[k] == (unsigned char)(250 + 7 * k));
}

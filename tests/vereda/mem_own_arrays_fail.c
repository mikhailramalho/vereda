// The arrays of their own an array indexed at a symbolic offset gets (#71)
// keep its unwritten elements unknown: a write through a pointer that may be
// either of two arrays leaves the other one as it was.
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned __VERIFIER_nondet_uint(void);

int main(void) {
  unsigned i = __VERIFIER_nondet_uint(), j = __VERIFIER_nondet_uint();
  unsigned k = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 4 && j < 4 && k < 3);
  int a[4], b[4], c[4];
  a[j] = 1;
  b[j] = 2;
  c[j] = 3;
  int *p = k == 0 ? a : (k == 1 ? b : c);
  p[i] = 9;
  __VERIFIER_assert(b[i] == 2 || b[i] == 9);
  return 0;
}

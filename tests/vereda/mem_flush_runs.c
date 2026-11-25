// A fully-known object flushed with several contiguous constant runs is
// compressed into a dominant-value constant array or range lambda, including
// when a later symbolic store writes on top of it.
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned __VERIFIER_nondet_uint(void);

int main(void) {
  int a[32];
  for (int k = 0; k < 10; ++k)
    a[k] = 3;
  for (int k = 10; k < 22; ++k)
    a[k] = 7;
  for (int k = 22; k < 32; ++k)
    a[k] = 11;

  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 32);
  int v = a[i];
  if (i < 10)
    __VERIFIER_assert(v == 3);
  else if (i < 22)
    __VERIFIER_assert(v == 7);
  else
    __VERIFIER_assert(v == 11);

  // A symbolic store on top of the compressed array.
  unsigned j = __VERIFIER_nondet_uint();
  __VERIFIER_assume(j < 32);
  a[j] = 99;
  if (i == j)
    __VERIFIER_assert(a[i] == 99);
  else if (i < 10)
    __VERIFIER_assert(a[i] == 3);

  // A byte array with runs.
  unsigned char b[24];
  for (int k = 0; k < 12; ++k)
    b[k] = 1;
  for (int k = 12; k < 24; ++k)
    b[k] = 2;
  unsigned m = __VERIFIER_nondet_uint();
  __VERIFIER_assume(m < 24);
  __VERIFIER_assert(b[m] == (m < 12 ? 1 : 2));
  return 0;
}

// A byte written through a pointer made from a nondet integer may land in
// an element of an array of int elements (#107).
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: line 18
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned __VERIFIER_nondet_uint(void);
extern unsigned long __VERIFIER_nondet_ulong(void);
int a[4];

int main(void) {
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 4);
  a[i] = 0x11223344;
  unsigned char *p = (unsigned char *)__VERIFIER_nondet_ulong();
  *p = 0x55;
  int v = a[i];
  __VERIFIER_assert(v != 0x11553344);
  return 0;
}

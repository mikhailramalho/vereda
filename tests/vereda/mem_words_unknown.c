// A pointer made from a nondet integer may point into an array of int
// elements (#107) that the walker found read and written only whole: a byte,
// or an int across two elements, written through it changes only the bytes
// it covers, and reads back through it.
// EXPECT: ^VERIFICATION SUCCESSFUL$
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
  __VERIFIER_assert(v == 0x11223344 || v == 0x11223355 || v == 0x11225544 ||
                    v == 0x11553344 || v == 0x55223344);
  __VERIFIER_assert(*p == 0x55);
  int *q = (int *)(__VERIFIER_nondet_ulong() | 1);
  *q = 0x66778899;
  __VERIFIER_assert(*q == 0x66778899);
  return 0;
}

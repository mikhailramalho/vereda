// Failing sibling of mem_own_large_unrolled: one element holds a different
// value, and a symbolic read must find it.
// EXPECT: ^VERIFICATION FAILED$
extern void __VERIFIER_assert(int);
extern void __VERIFIER_assume(int);
extern unsigned __VERIFIER_nondet_uint(void);

#define SET4(a, k, v) a[k] = v; a[(k)+1] = v; a[(k)+2] = v; a[(k)+3] = v;
#define SET16(a, k, v) SET4(a, k, v) SET4(a, (k)+4, v) SET4(a, (k)+8, v) SET4(a, (k)+12, v)
#define SET64(a, k, v) SET16(a, k, v) SET16(a, (k)+16, v) SET16(a, (k)+32, v) SET16(a, (k)+48, v)

int main(void) {
  int a[128];
  SET64(a, 0, 7)
  SET64(a, 64, 7)
  a[105] = 8;

  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 128);
  __VERIFIER_assert(a[i] == 7);
  return 0;
}

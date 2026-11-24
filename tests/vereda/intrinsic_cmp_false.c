// llvm.ucmp compares as unsigned: ucmp(u, 0) is 1 for every u from 2^31 up,
// where a signed compare would give -1.
// EXPECT: ^VERIFICATION FAILED$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assert(int);

static int ucmp(unsigned x, unsigned y) { return (x > y) - (x < y); }

int main(void) {
  unsigned u = __VERIFIER_nondet_uint();
  __VERIFIER_assert(ucmp(u, 0) != 1 || u < 0x80000000u);
  return 0;
}

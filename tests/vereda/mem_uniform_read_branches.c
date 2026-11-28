// A uniform read on one branch cannot hide a different value on its sibling.
// RUN: -m32
// RUN: -m64
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

unsigned char bytes[64] = {[31] = 9};

__attribute__((noinline)) unsigned char read_at(unsigned char *p, unsigned i) {
  return p[i];
}

int main(void) {
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < sizeof bytes);
  if (i < 31)
    __VERIFIER_assert(read_at(bytes, i) == 0);
  else
    __VERIFIER_assert(read_at(bytes, i) == (i == 31 ? 9 : 0));
}

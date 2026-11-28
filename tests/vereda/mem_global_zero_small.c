// A helper's symbolic read must see every byte of a small zeroed global.
// The helper prevents ownObject from giving the global offset-indexed arrays.
// RUN: -m32
// RUN: -m64
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

unsigned char bytes[1000];

__attribute__((noinline)) unsigned char read_at(unsigned char *p, unsigned i) {
  return p[i];
}

int main(void) {
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < sizeof bytes);
  __VERIFIER_assert(read_at(bytes, i) == 0);
}

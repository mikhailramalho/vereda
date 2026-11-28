// A write through an alias must override the zeroed global's constant array.
// RUN: -m32
// RUN: -m64
// EXPECT: ^VERIFICATION FAILED$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

unsigned char bytes[1000];

__attribute__((noinline)) void write_at(unsigned char *p, unsigned i) {
  p[i] = 9;
}

int main(void) {
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < sizeof bytes);
  write_at(bytes, i);
  __VERIFIER_assert(bytes[i] == 0);
}

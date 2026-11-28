// The range includes an unknown byte beyond the initialized object. Its
// uniform known prefix must not make the entire symbolic read zero.
// RUN: -m32
// RUN: -m64
// EXPECT: ^VERIFICATION FAILED$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

unsigned char bytes[32];

__attribute__((noinline)) unsigned char read_at(unsigned char *p, unsigned i) {
  return p[i];
}

int main(void) {
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i <= sizeof bytes);
  __VERIFIER_assert(read_at(bytes, i) == 0);
}

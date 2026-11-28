// Uniform multi-byte values and pointers fold without losing their provenance.
// RUN: -m32
// RUN: -m64
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

unsigned numbers[32] = {[0 ... 31] = 0x12345678u};
int value = 17;
int *pointers[32] = {[0 ... 31] = &value};

__attribute__((noinline)) unsigned read_number(unsigned *p, unsigned i) {
  return p[i];
}

__attribute__((noinline)) int *read_pointer(int **p, unsigned i) {
  return p[i];
}

int main(void) {
  unsigned i = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 32);
  __VERIFIER_assert(read_number(numbers, i) == 0x12345678u);
  int *p = read_pointer(pointers, i);
  __VERIFIER_assert(p == &value && *p == 17);
}

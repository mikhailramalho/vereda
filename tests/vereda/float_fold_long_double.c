// long double (x86_fp80) is held as bits and its arithmetic is unsupported;
// constants of it do not fold either, so the answer does not depend on
// whether the operands happen to be constants (float_long_double_cmp.c).
// RUN: -m64
// EXPECT: ^VERIFICATION UNKNOWN: unsupported: type x86_fp80$
extern void __VERIFIER_assert(int);
long double in[] = {1.0L, 3.0L};
double d = 0.1;
int main(void) {
  __VERIFIER_assert((long double)d != in[0] / in[1]);
  return 0;
}

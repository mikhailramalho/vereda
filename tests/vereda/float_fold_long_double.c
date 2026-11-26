// Constant folding on long double (x86_fp80) uses APFloat::x87DoubleExtended,
// matching the solver's (15, 63) encoding.
// RUN: -m64
// RUN: -m64 --fp2bv
// EXPECT: ^VERIFICATION SUCCESSFUL$
extern void __VERIFIER_assert(int);
long double in[] = {1.0L, 3.0L};
double d = 0.1;
int main(void) {
  __VERIFIER_assert((long double)d != in[0] / in[1]);
  return 0;
}

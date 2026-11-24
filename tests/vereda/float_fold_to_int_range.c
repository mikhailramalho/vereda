// A constant float out of an integer type's range converts as the encoding
// converts any such float (LLVM makes it poison): the fold does not take
// APFloat's saturated value, and the solver finds another.
// RUN: --unwind 2 --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  FAILED +\[main\.assertion\.1\]  line 11
// EXPECT: ^  FAILED +\[main\.assertion\.2\]  line 12
extern void __VERIFIER_assert(int);
double in[] = {3e9, -1.0};
int main(void) {
  __VERIFIER_assert((int)in[0] == 2147483647);
  __VERIFIER_assert((unsigned)in[1] == 0);
  return 0;
}

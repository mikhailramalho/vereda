// The fold keeps -0.0's sign: 1/-0.0 is minus infinity, so the second
// assertion fails (float_fold_signed_zero.c).
// RUN: --unwind 3
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: line 11
extern void __VERIFIER_assert(int);
double in[] = {0.0, 1.0};
int main(void) {
  double pz = in[0] * in[1], nz = -pz;
  __VERIFIER_assert(pz == nz);
  __VERIFIER_assert(1 / pz == 1 / nz);
  return 0;
}

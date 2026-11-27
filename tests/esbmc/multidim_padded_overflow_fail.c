// Padded to powers of two, this array has 2^64 elements: rows must not alias
#define K 16385
unsigned __VERIFIER_nondet_uint(void);
char a[16][K][K][K][K];
int main(void){
  unsigned i = __VERIFIER_nondet_uint(), z = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i == 1 && z == 0);
  a[z][0][0][0][0] = 1;
  a[i][0][0][0][0] = 2;
  __VERIFIER_assert(a[z][0][0][0][0] == 2); // a[0] holds 1
  return 0;
}
// Imported from ESBMC regression/esbmc/multidim_padded_overflow_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$

// Padded to powers of two, this array has 2^64 elements: rows must not alias
#define K 16385
unsigned __VERIFIER_nondet_uint(void);
char a[16][K][K][K][K];
int main(void){
  unsigned i = __VERIFIER_nondet_uint(); __VERIFIER_assume(i < 16);
  a[0][0][0][0][0] = 1;
  __VERIFIER_assert(a[i][0][0][0][0] == (i == 0)); // distinct rows
  return 0;
}
// Imported from ESBMC regression/esbmc/multidim_padded_overflow
// EXPECT: ^VERIFICATION SUCCESSFUL$

int __VERIFIER_nondet_int();
int main() {
  int i, j=__VERIFIER_nondet_int();
  i = 2147483640 ;
  i = i+j;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_277_2
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$

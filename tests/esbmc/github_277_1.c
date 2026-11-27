long __VERIFIER_nondet_long(void);

int main(void) {
  long l = 0;
  int c = 0;
  long t = __VERIFIER_nondet_long();
  long s = __VERIFIER_nondet_long();
  long *b = &s;

  l = (t + *b) & (0xffffffffL); 
  c += (l < t);
}
// Imported from ESBMC regression/esbmc/github_277_1
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$

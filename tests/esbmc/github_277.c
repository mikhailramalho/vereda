long __VERIFIER_nondet_long(void);

int main(void) {
  long l = 0;
  int c = 0;
  long t = __VERIFIER_nondet_long();
  c += (l < t);
}
// Imported from ESBMC regression/esbmc/github_277
// RUN: --overflow-check
// EXPECT: ^VERIFICATION SUCCESSFUL$

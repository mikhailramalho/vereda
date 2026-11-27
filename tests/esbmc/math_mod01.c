int main(void) {
  unsigned int i = 101;
  unsigned int j = 100;
  unsigned k = (int) i % (int) j;
  return 0;
}

// Imported from ESBMC regression/esbmc/math_mod01
// RUN: --ub-pointer-check=false --ub-div-by-zero-check=false --ub-signed-overflow-check --ub-shift-check --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$

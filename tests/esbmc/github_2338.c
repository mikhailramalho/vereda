int main() {
  unsigned int a = 0x80000000;
  return -a;
}
// Imported from ESBMC regression/esbmc/github_2338
// RUN: --ub-signed-overflow-check --unsigned-overflow-check --ub-shift-check
// EXPECT: ^VERIFICATION FAILED$

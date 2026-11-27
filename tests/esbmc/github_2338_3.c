int main() {
  unsigned int a = 0;
  return -a;
}
// Imported from ESBMC regression/esbmc/github_2338_3
// RUN: --ub-signed-overflow-check --unsigned-overflow-check --ub-shift-check
// EXPECT: ^VERIFICATION SUCCESSFUL$

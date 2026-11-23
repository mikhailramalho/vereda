int main() {
  unsigned int a = 0;
  return -a;
}
// Imported from ESBMC regression/esbmc/github_2338_3
// RUN: --unsigned-overflow-check
// EXPECT: ^VERIFICATION SUCCESSFUL$

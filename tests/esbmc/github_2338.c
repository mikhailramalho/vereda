int main() {
  unsigned int a = 0x80000000;
  return -a;
}
// Imported from ESBMC regression/esbmc/github_2338
// RUN: --unsigned-overflow-check
// EXPECT: ^VERIFICATION FAILED$

int main() {
  int a;
  int b = (int) &a;
  int *c = (int*) b;
  *c = 42;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_2673_roundtrip
// EXPECT: ^VERIFICATION SUCCESSFUL$

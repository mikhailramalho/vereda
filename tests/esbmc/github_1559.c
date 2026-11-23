int main()
{
  unsigned int x = 0;
  unsigned int y = 2;
  unsigned int z = y - x;
}
// Imported from ESBMC regression/esbmc/github_1559
// RUN: --unsigned-overflow-check --solver z3
// EXPECT: ^VERIFICATION SUCCESSFUL$

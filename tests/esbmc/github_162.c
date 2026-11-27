union {
  unsigned a : 3
} b;

int main()
{
  b.a;
}
// Imported from ESBMC regression/esbmc/github_162
// EXPECT: ^VERIFICATION SUCCESSFUL$

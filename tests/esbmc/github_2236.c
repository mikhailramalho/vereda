struct {
  int a;
} b[];
void c() { b->a; }
int main()
{
  return 0;
}
// Imported from ESBMC regression/esbmc/github_2236
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$

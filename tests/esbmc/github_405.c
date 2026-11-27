struct a
{
  short symbol;
};
struct d
{
  struct a f;
};
int main()
{
  struct d *g;
  g->f;
  return 0;
}
// Imported from ESBMC regression/esbmc/github_405
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL

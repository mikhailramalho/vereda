int *p = &(int){0};
extern int *q;
int main()
{
  *q = 5;
  assert(*p == 0);
}
// Imported from ESBMC regression/esbmc/github_591-multiple-files
// RUN: --no-incremental-bmc
// FILES: b.c
// EXPECT: ^VERIFICATION SUCCESSFUL$

extern void fill(int *p);

int main(void)
{
  int a[5];
  fill(&a[0]);
  return a[2];
}
// Imported from ESBMC regression/esbmc/cwe_uninit_array_addrof
// RUN: --uninitialised-vars-check --incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$

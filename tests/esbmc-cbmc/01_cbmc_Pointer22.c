void f(int *);

int main()
{
  int a[5];

  f(a);
}

void f(int *p)
{
  p[10]=0;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Pointer22
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL

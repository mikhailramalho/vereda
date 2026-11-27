int main()
{
  int i;
  int k;

  for (i=0; i<3; i++)
  {
    k=3;
    continue;
    k=4;
  }

  assert(0);
}
// Imported from ESBMC regression/cbmc/01_cbmc_for2
// EXPECT: ^VERIFICATION FAILED$

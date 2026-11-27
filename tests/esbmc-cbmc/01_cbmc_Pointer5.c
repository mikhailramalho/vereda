int a[]={ 1, 2, 3 };

int main()
{
  assert((a-1)[2]==2);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Pointer5
// EXPECT: ^VERIFICATION SUCCESSFUL$

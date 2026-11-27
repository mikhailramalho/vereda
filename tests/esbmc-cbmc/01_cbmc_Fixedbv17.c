int main()
{
  float a[2];
  int temp;
  a[0] = -0.375f;
	temp = (int)(a[0]/0.03125f);
	assert(temp==-12);
}

// Imported from ESBMC regression/cbmc/01_cbmc_Fixedbv17
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$

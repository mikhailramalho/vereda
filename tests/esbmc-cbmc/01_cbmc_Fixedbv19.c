int main()
{
  float a[2], temp;
  a[0] = -0.375f;
	temp = (int)(a[0]/0.03125f);
	assert(temp==-12.0f);
}

// Imported from ESBMC regression/cbmc/01_cbmc_Fixedbv19
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$

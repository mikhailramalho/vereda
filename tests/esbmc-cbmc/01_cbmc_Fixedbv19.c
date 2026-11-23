int main()
{
  float a[2], temp;
  a[0] = -0.375f;
	temp = (int)(a[0]/0.03125f);
	assert(temp==-12.0f);
}

// Imported from ESBMC regression/cbmc/01_cbmc_Fixedbv19
// Needs: --fixedbv
// RUN: --fixedbv --no-slice -Wno-error=implicit-function-declaration
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

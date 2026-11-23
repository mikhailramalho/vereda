int main()
{
 int i;
 int xsize = 1;
 float y[xsize];

 for (i = 0; i<xsize; ++i)
 {
   y[i] = 0;
 }
 assert(0);
}

// Imported from ESBMC regression/cbmc/01_cbmc_Fixedbv12
// Needs: --fixedbv REQUIRES arch32
// RUN: --fixedbv --no-slice --32 -Wno-error=implicit-function-declaration
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none

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
// RUN: -m32
// EXPECT: ^VERIFICATION FAILED$

int main()
{
  __complex__ double z = 1.0 + 2.0i;
  __complex__ double c = ~z;
  assert(__imag__ c == 2.0);
  return 0;
}
// Imported from ESBMC regression/esbmc/complex_24
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$

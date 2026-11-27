int main()
{
  __complex__ double x;
  __real__ x = 3.0;
  __imag__ x = 4.0;

  assert(__real__ x == 3.0);
  assert(__imag__ x == 4.0);

  return 0;
}
// Imported from ESBMC regression/esbmc/complex_01
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$

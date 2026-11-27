int main()
{
  __complex__ double x = 2.0i;

  assert(__real__ x == 0.0);
  assert(__imag__ x == 99.0);

  return 0;
}
// Imported from ESBMC regression/esbmc/complex_04
// EXPECT: ^VERIFICATION FAILED$

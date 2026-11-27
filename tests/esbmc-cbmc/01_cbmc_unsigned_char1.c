char x;
signed char sx;

int main ()
{
  x=-1;
  // this will only work if 'char' is unsigned
  assert(x==255);

  // a regular one
  sx=-1;
  assert(sx==-1);
}
// Imported from ESBMC regression/cbmc/01_cbmc_unsigned_char1
// Needs: -funsigned-char
// RUN: -funsigned-char --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

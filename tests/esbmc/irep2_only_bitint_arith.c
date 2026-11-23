/* Clang inserts the usual arithmetic conversions itself for the ordinary
   integer types, so a bit-precise operand is where the adjuster's own
   conversion is observable. */
int main(void)
{
  _ExtInt(10) x = 1;
  _ExtInt(10) y = 2;
  _ExtInt(10) z = x + y;
  return (int)z;
}
// Imported from ESBMC regression/esbmc/irep2_only_bitint_arith
// Needs: --goto-functions-only --clang-c-irep2-adjust-only
// RUN: --goto-functions-only --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: \(signed int\)x \+ \(signed int\)y
// Vereda: verdict none

/* C11 6.5.16.2p3: `b op= a` is `b = b op (a)`, so a narrow target promotes
   before the operation. Without the conversion the lowered add2t/sub2t are
   built with mismatched widths. */
char b;

int main(void)
{
  int a = 2;

  b = 3;
  b += a;
  b -= a;
  b *= a;
  b &= a;
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_compound_assign
// Needs: --clang-c-irep2-adjust-only --symbol-table-only
// RUN: --clang-c-irep2-adjust-only --symbol-table-only
// MODE: FUTURE
// EXPECT: ^\(signed int\)b \+= a;$
// EXPECT: ^\(signed int\)b -= a;$
// EXPECT: ^\(signed int\)b \*= a;$
// EXPECT: ^\(signed int\)b &= a;$
// Vereda: verdict none

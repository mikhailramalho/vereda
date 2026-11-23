/* #10008: printing this initialiser read past its operands. */
int main(void)
{
  union { struct { int s; int c; }; unsigned i; } v = { .s = 17 };
  return v.i;
}
// Imported from ESBMC regression/esbmc/github_10008_print
// Needs: --goto-functions-only
// RUN: --goto-functions-only
// MODE: FUTURE
// EXPECT: ^ +ASSIGN v=\{ \.[^=]+=\{ \.s=17, \.c=0 \} \};$
// Vereda: verdict none

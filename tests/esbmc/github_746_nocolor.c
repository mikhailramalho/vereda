/* Pins that the clang AST dump the frontend emits for an unsupported
   construct honours --color never. The construct is incidental -- it was an
   indirect goto until that became supported (issue #4083). */
_Atomic int a;

int main(void)
{
  __c11_atomic_fetch_nand(&a, 1, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_746_nocolor
// Needs: --color never
// RUN: --color never
// MODE: FUTURE
// EXPECT: ^.*CONVERSION ERROR$
// EXPECT: \A(?!(?s:.)*\x1b\[0;1;35m)
// Vereda: verdict none

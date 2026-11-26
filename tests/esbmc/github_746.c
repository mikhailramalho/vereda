/* THOROUGH, i.e. Linux-only: on Windows LLVM's prepare_colors() refuses colour
   for any stream that is not a console (Process::ColorNeedsFlush()), and the
   dump goes through a raw_os_ostream, so the escapes never appear there.
   github_746_nocolor pins the absence case on every platform.

   The construct is incidental -- it only has to be one the frontend cannot
   convert, so that it dumps the clang AST. It was an indirect goto until that
   became supported (issue #4083). */
_Atomic int a;

int main(void)
{
  __c11_atomic_fetch_nand(&a, 1, 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_746
// Needs: always
// RUN: always
// MODE: FUTURE
// EXPECT: ^.*CONVERSION ERROR$
// EXPECT: \x1b\[0;1;35mAtomicExpr
// Vereda: verdict none

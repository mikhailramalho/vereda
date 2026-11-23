/* The loop variable's scope is the loop, so its DEAD belongs after the body
   that reads it. clang_c_adjust gets that by hoisting the init into an
   enclosing block; the hoist must splice a block-shaped init rather than
   nesting it, or the scope closes immediately. */
int main(void)
{
  int s = 0;
  for (int i = 0; i < 3; i++)
    s += i;
  return s;
}
// Imported from ESBMC regression/esbmc/irep2_only_for_scope
// Needs: --goto-functions-only --clang-c-irep2-adjust-only
// RUN: --goto-functions-only --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: ^ *2: DEAD c:irep2_only_for_scope\.c@[0-9]*@F@main@i$
// Vereda: verdict none

/* No <assert.h>: `assert` is implicitly declared, so the adjuster synthesises
   the callee symbol. A sideeffect2t carries no location of its own, so the
   symbol's has to come from the enclosing statement. */
int main(void)
{
  int x = 1;

  assert(x == 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_implicit_callee_location
// Needs: --clang-c-irep2-adjust-only --symbol-table-only
// RUN: --clang-c-irep2-adjust-only --symbol-table-only
// MODE: FUTURE
// EXPECT: ^Symbol\.+: c:@F@assert$
// EXPECT: ^Location\.+: file .*irep2_only_implicit_callee_location\.c line 8 column 3 function main$
// Vereda: verdict none

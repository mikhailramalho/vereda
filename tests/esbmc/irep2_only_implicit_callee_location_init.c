/* Sibling of irep2_only_implicit_callee_location, where the undeclared call is
   an initialiser rather than the whole statement. sideeffect2t now carries its
   own location; before it did, this symbol had none, and the enclosing
   statement's would have named the column of `int`, not of the callee. */
int main(void)
{
  int x = undeclared_fn(1);
  return x;
}
// Imported from ESBMC regression/esbmc/irep2_only_implicit_callee_location_init
// Needs: --clang-c-irep2-adjust-only --symbol-table-only
// RUN: --clang-c-irep2-adjust-only --symbol-table-only
// MODE: FUTURE
// EXPECT: ^Symbol\.+: c:@F@undeclared_fn$
// EXPECT: ^Location\.+: file .*irep2_only_implicit_callee_location_init\.c line 7 column 11 function main$
// Vereda: verdict none

/* The same call on an assignment's right-hand side. The statement here is an
   expression statement whose operand is the assignment, so the pre-recursion
   path sees the assignment rather than the call: the location has to come off
   the sideeffect2t. */
int main(void)
{
  int x;
  x = undeclared_fn(1);
  return x;
}
// Imported from ESBMC regression/esbmc/irep2_only_implicit_callee_location_assign
// Needs: --clang-c-irep2-adjust-only --symbol-table-only
// RUN: --clang-c-irep2-adjust-only --symbol-table-only
// MODE: FUTURE
// EXPECT: ^Symbol\.+: c:@F@undeclared_fn$
// EXPECT: ^Location\.+: file .*irep2_only_implicit_callee_location_assign\.c line 8 column 7 function main$
// Vereda: verdict none

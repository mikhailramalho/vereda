/* Nested expression statements: the walk restores the enclosing statement's
   location on the way out, so a call in a loop body does not inherit the one
   belonging to a statement it is not inside. */
int main(void)
{
  int a = 0;

  outer(a);

  while (a)
  {
    inner(a);
    a = 0;
  }

  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_implicit_callee_location_stmt
// Needs: --clang-c-irep2-adjust-only --symbol-table-only
// RUN: --clang-c-irep2-adjust-only --symbol-table-only
// MODE: FUTURE
// EXPECT: ^Symbol\.+: c:@F@outer$
// EXPECT: ^Location\.+: file .*irep2_only_implicit_callee_location_stmt\.c line 8 column 3 function main$
// EXPECT: ^Symbol\.+: c:@F@inner$
// EXPECT: ^Location\.+: file .*irep2_only_implicit_callee_location_stmt\.c line 12 column 5 function main$
// Vereda: verdict none

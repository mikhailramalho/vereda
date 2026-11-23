/* Pins the v1 erase path of goto_loop_simplify: a for loop whose
 * induction variable is local-scoped (DEAD immediately after exit)
 * gets eliminated entirely. Verification produces zero VCCs because
 * the loop is gone before symex runs. */
int main()
{
  for (int i = 0; i < 17; i++)
    ;
  return 0;
}
// Imported from ESBMC regression/esbmc/loop_simplify_erase_dead_for
// RUN: --unwind 1 --no-unwinding-assertions
// EXPECT: ^VERIFICATION SUCCESSFUL$

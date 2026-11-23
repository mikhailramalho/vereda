/* v1 erase path for the do-while shape. Distinct from for/while: the
 * back-edge is conditional (IF cond GOTO loop_head) and there's no
 * leading exit IF, so shape discrimination has to recognise this and
 * still find the dead variables. */
int main()
{
  {
    int i = 0;
    do {
      i++;
    } while (i < 17);
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/loop_simplify_erase_dead_dowhile
// RUN: --unwind 1 --no-unwinding-assertions
// EXPECT: ^VERIFICATION SUCCESSFUL$

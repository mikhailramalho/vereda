/* take_ptr is declared, not defined: clang leaves the array-to-pointer decay
   for the adjuster rather than inserting it in the AST. */
void take_ptr(int *p);

int main(void)
{
  int a[3];
  a[0] = 1;
  take_ptr(a);
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_call_arg_decay
// Needs: --goto-functions-only --clang-c-irep2-adjust-only
// RUN: --goto-functions-only --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: FUNCTION_CALL: +take_ptr\(&a\[0\]\)
// Vereda: verdict none

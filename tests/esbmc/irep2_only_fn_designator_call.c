/* `f(x)` and `(&f)(x)` reach goto_convert with the same shape; only the
   implicit bit tells the sugar from a user-written function pointer, and a
   direct call must not become a dereference of it. */
static int callee(int x)
{
  return x + 1;
}

int main(void)
{
  __VERIFIER_assert(callee(1) == 2); // a direct call stays direct
  __VERIFIER_assert((&callee)(1) == 2); // and so does one through a written &f
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_fn_designator_call
// Needs: --clang-c-irep2-adjust-only
// RUN: --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

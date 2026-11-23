/* C11 6.3.2.1p4: dereferencing a pointer to a function yields a function
   designator, which converts straight back to a pointer -- so *f is f, and
   ******f too. Left bare, the code-typed dereference reaches a consumer that
   wants a pointer. */
int f(void)
{
  return 7;
}

int main(void)
{
  int (*p)(void) = &f;

  __VERIFIER_assert(*f == &f); // *f is f
  __VERIFIER_assert(***f == &f); // repeated dereference is still f
  __VERIFIER_assert(*p == &f); // through a pointer variable too
  __VERIFIER_assert((*f)() == 7); // and it is still callable
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_func_deref
// Needs: --clang-c-irep2-adjust-only
// RUN: --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

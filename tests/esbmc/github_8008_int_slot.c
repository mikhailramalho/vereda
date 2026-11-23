// #8008: a pointer stored into an integer slot through a pointer.
int main()
{
  int x = 1;
  long cell;
  long *lp = &cell;
  *lp = (long)&x;
  int *q = (int *)*lp;
  int a = x + 1;
  __VERIFIER_assert(*q == 1); // q aliases x
  int b = x + 1;
  __VERIFIER_assert(a == b); // x unchanged
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8008_int_slot
// Needs: --gcse
// RUN: --gcse
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

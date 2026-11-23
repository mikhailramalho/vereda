/* The base case reaches the violation after the loop (esbmc/esbmc#1320). */
int __VERIFIER_nondet_int();
_Bool __VERIFIER_nondet_bool();
int x;

int main()
{
  x = __VERIFIER_nondet_int();
  do
  {
    if (__VERIFIER_nondet_bool())
      x--;
    else
      x -= 2;
  } while (x > 0);
  __VERIFIER_assert(x != 0); // the loop can leave with x == 0
  return 0;
}
// Imported from ESBMC regression/k-induction/github_1320_do_while_fail
// Needs: --k-induction
// RUN: --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none

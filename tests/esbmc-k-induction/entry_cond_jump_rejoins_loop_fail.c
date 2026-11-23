// The c != 0 path jumps past the back edge and back into the body at mid, so
// that jump does not leave the loop and c == 0 must not be assumed.
int __VERIFIER_nondet_int();
int main()
{
  int y = 0;
  int c = __VERIFIER_nondet_int();
  while (1)
  {
    __VERIFIER_assert(!(c && y == 15)); // y != 15
    y++;
    if (c)
    {
      c = c;
      goto far;
    }
  mid:
    if (y > 1000000)
      break;
  }
  return 0;
far:
  goto mid;
}
// Imported from ESBMC regression/k-induction/entry_cond_jump_rejoins_loop_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 40
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none

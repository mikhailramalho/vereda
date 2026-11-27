/* #9786: a call through a pointer that has a target still disables the
 * inductive step. */
int cb(int x)
{
  return x;
}

int (*fp)(int) = cb;

int main(void)
{
  fp(1);
  unsigned i = 0;
  while (1)
  {
    i = (i + 1) % 4;
    __VERIFIER_assert(i < 4); // i stays below 4
  }
  return 0;
}
// Imported from ESBMC regression/k-induction/github_9786_fnptr_with_target
// Needs: --k-induction
// RUN: --k-induction --max-k-step 6 --ub-pointer-check=false
// MODE: FUTURE
// EXPECT: ^WARNING: k-induction does not support function pointer calls yet\. Disabling inductive step$
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none

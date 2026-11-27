// The assume runs before the nondet, so it reads the previous input. a == 1
// is reached only by input 1, so every iteration at a == 1 is cut; input 2 at
// a == 0 exits. No infinite execution exists.
int a = 0;
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void exit(int);
int update(int input)
{
  if (a == 0 && input == 1) { a = 1; return 1; }
  if (a == 1 && input == 2) { a = 0; return 2; }
  exit(0);
  return -2;
}
int main(void)
{
  int sink = 0;
  int input = 0;
  while (1)
  {
    __VERIFIER_assume(!(a == 1 && input == 1));
    input = __VERIFIER_nondet_int();
    if (input != 1 && input != 2)
      return -2;
    sink = update(input);
  }
  return sink;
}
// Imported from ESBMC regression/termination/lasso_eca_period2_prev_input
// Needs: --termination --max-inductive-step 3 --interval-analysis
// RUN: --ub-div-by-zero-check=false --no-align-check --k-step 2 --unlimited-k-steps --vla-size-check=false -m32 --ub-pointer-check=false --no-bounds-check --no-assertions --termination --max-inductive-step 3 --interval-analysis
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none

// As lasso_eca_period2, but input 2 moves a to 2, where every input
// exits: no state repeats on a run from the initial one.

int a = 0;

extern int __VERIFIER_nondet_int(void);
extern void exit(int);

int update(int input)
{
  if (a == 0 && input == 1)
  {
    a = 1;
    return 1;
  }
  if (a == 1 && input == 2)
  {
    a = 2;
    return 2;
  }
  if (a == 0 || a == 1)
    exit(0);
  if (a != 0)
    exit(0);
  return -2;
}

int main(void)
{
  int sink = 0;
  while (1)
  {
    int input;
    input = __VERIFIER_nondet_int();
    if (input != 1 && input != 2)
      return -2;
    sink = update(input);
  }
  return sink;
}
// Imported from ESBMC regression/termination/lasso_eca_period2_exits
// Needs: --no-assertions --termination --max-inductive-step 3 --interval-analysis
// RUN: --no-div-by-zero-check --no-align-check --k-step 2 --unlimited-k-steps --no-vla-size-check -m32 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3 --interval-analysis
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none

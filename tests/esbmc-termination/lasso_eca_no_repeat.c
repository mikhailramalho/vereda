// Every input raises c, so no state repeats, and a run exits once c reaches
// 60, after 30 iterations or more: past the 24 the concrete cycle search
// covers, so the solver decides, and finds no cycle either.

int c = 0;

extern int __VERIFIER_nondet_int(void);
extern void exit(int);

int update(int input)
{
  if (c >= 60)
    exit(0);
  if (input == 1)
  {
    c = c + 1;
    return 1;
  }
  c = c + 2;
  return 2;
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
// Imported from ESBMC regression/termination/lasso_eca_no_repeat
// Needs: --termination --max-inductive-step 3 --interval-analysis
// RUN: --ub-div-by-zero-check=false --no-align-check --k-step 2 --unlimited-k-steps --vla-size-check=false -m32 --ub-pointer-check=false --no-bounds-check --no-assertions --termination --max-inductive-step 3 --interval-analysis
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none

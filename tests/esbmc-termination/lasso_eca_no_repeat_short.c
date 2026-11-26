// Every input raises c, so no state repeats, and a run exits once c reaches
// 20, within 20 iterations: the concrete cycle search sees every reachable
// state, finds no cycle and needs no solver.

int c = 0;

extern int __VERIFIER_nondet_int(void);
extern void exit(int);

int update(int input)
{
  if (c >= 20)
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
// Imported from ESBMC regression/termination/lasso_eca_no_repeat_short
// Needs: --no-assertions --termination --max-inductive-step 3 --interval-analysis
// RUN: --no-div-by-zero-check --no-align-check --k-step 2 --unlimited-k-steps --no-vla-size-check -m32 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3 --interval-analysis
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none

// Real: update always sees x == 1 ((unsigned char)257 == 1), a climbs to 4,
// exit. Every execution terminates.
int a = 0;
extern int __VERIFIER_nondet_int(void);
extern void exit(int);
int update(int x)
{
  if (x == 257)
  {
    a = 1 - a;
    return 1;
  }
  if (x == 1)
  {
    a = a + 1;
    if (a > 3)
      exit(0);
    return 2;
  }
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
    if (input != 1 && input != 257)
      return -2;
    sink = update((unsigned char)input);
  }
  return sink;
}
// Imported from ESBMC regression/termination/lasso_eca_period2_cast
// Needs: --termination --max-inductive-step 3 --interval-analysis
// RUN: --no-div-by-zero-check --no-align-check --k-step 2 --unlimited-k-steps --no-vla-size-check --32 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3 --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none

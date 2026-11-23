// The loop's assume cuts the only state where no arm fires, so every
// execution ends (#10005).
int a = 0;
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void exit(int);
int update(int input)
{
  if (a == 0 && input == 1) { a = 1; return 1; }
  if (a == 0) exit(0);
  return -2;
}
int main(void)
{
  int sink = 0;
  while (1)
  {
    __VERIFIER_assume(a != 1);
    int input;
    input = __VERIFIER_nondet_int();
    if (input != 1 && input != 2)
      return -2;
    sink = update(input);
  }
  return sink;
}
// Imported from ESBMC regression/esbmc/nonterm_eca_loop_assume
// Needs: --termination --max-inductive-step 3
// RUN: --termination --max-inductive-step 3 --k-step 2 --unlimited-k-steps
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$

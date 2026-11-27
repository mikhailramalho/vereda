/* A struct element is left to unwinding: a lambda over a struct array
   reaches the solver's tuple flattening, which aborted. */
typedef struct { double c[2]; int k; } P;
P pts[3];
int __VERIFIER_nondet_int(void);
int main()
{
  P old[3];
  for (int i = 0; i < 3; i++)
    old[i] = pts[i];
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x >= 0 && x < 3);
  old[x].k++;
  int y = __VERIFIER_nondet_int();
  __VERIFIER_assume(y >= 0 && y < 3);
  __VERIFIER_assert(old[y].k <= 1);
  return 0;
}
// Imported from ESBMC regression/esbmc/array_loop_summary_struct
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$

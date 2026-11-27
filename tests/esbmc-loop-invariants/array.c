unsigned int __VERIFIER_nondet_uint();
int __VERIFIER_nondet_int();

int main()
{
  unsigned int SIZE = 1;
  unsigned int j, k;
  int array[SIZE], menor;

  menor = __VERIFIER_nondet_int();

  __ESBMC_loop_invariant(j <= SIZE && (j == 0 || menor <= array[0]));
  for (j = 0; j < SIZE; j++)
  {
    array[j] = __VERIFIER_nondet_int();

    if (array[j] <= menor)
      menor = array[j];
  }

  assert(array[0] >= menor);
}
// Imported from ESBMC regression/loop-invariants/array
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$

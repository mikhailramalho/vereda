unsigned int __VERIFIER_nondet_uint();
int __VERIFIER_nondet_int();

#define SIZE 1

int main()
{
  unsigned int j, k;
  int array[SIZE], menor;

  menor = __VERIFIER_nondet_int();

  __ESBMC_loop_invariant(j == 0 || menor == array[0]);
  for (j = 0; j < SIZE; j++)
  {
    array[j] = __VERIFIER_nondet_int();

    if (array[j] <= menor)
      menor = array[j];
  }

  assert(array[0] > menor);
}
// Imported from ESBMC regression/loop-invariants/array_bug
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check --multi-property --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$

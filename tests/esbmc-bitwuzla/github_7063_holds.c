#define SIZE 3
typedef char data_t;
typedef int idx_t;

int main()
{
  idx_t idx;
  data_t vec[SIZE];
  idx_t idx_vfy;

  idx = 0;
  __ESBMC_loop_invariant(
    0 <= idx && idx <= SIZE - 1 &&
    __ESBMC_forall(
      &idx_vfy, !(0 <= idx_vfy && idx_vfy < idx) || (vec[idx_vfy] == 3)));

  while (idx < SIZE - 1)
  {
    vec[idx] = 3;
    ++idx;
  }
}
// Imported from ESBMC regression/bitwuzla/github_7063_holds
// Needs: --loop-invariant-check
// RUN: --multi-property --loop-invariant-check --bitwuzla
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: ^  PASSED +\[main\.assertion\.1\] +line 17 +loop invariant base case$
// Vereda: verdict none

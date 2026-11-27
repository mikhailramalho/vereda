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
    __ESBMC_forall(&idx_vfy, !(idx_vfy == 1) || (vec[idx_vfy] == 17)));

  while (idx < SIZE - 1)
  {
    vec[idx] = 3;
    ++idx;
  }
}
// Imported from ESBMC regression/bitwuzla/github_7063
// Needs: __ESBMC_forall, __ESBMC_loop_invariant
// Needs: --loop-invariant-check
// RUN: --multi-property --loop-invariant-check --bitwuzla --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^Violated property:$
// EXPECT: ^  FAILED +\[main\.assertion\.1\] +line 15 +loop invariant base case$
// EXPECT: ^  UNKNOWN +\[main\.array-bounds-violated\.1\] +line 17 .*loop invariant too weak
// EXPECT: \A(?![\s\S]*Can't get boolean value)[\s\S]*\Z

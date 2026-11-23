/* github #9228: an object's alignment is a power of two, so its base address
 * is constrained by zeroing its low bits rather than by a bvurem. */
#include <stdint.h>
#include <stdlib.h>

_Alignas(32) char buf[3];

int main(void)
{
  int *p = malloc(sizeof(int));
  __VERIFIER_assert((uintptr_t)buf != (uintptr_t)p); // distinct objects
  return 0;
}
// Imported from ESBMC regression/smtlib/github_9228
// Needs: --smtlib --output out.smt2 CHECK_FILE out.smt2 contains \(\(_ extract 4 0\) \|__ESBMC_ptr_obj_start_\d+\|\) CHECK_FILE out.smt2 contains \(\(_ extract 3 0\) \|__ESBMC_ptr_obj_start_\d+\|\) CHECK_FILE out.smt2 absent bvurem
// RUN: --smtlib --smt-formula-only --output out.smt2 --64
// MODE: FUTURE
// EXPECT: ^SMT formula written to output file out\.smt2$
// Vereda: verdict none

#include "../symex_ssa_00/symex_ssa_00.c"
// Imported from ESBMC regression/esbmc/symex_ssa_00_fail
// Needs: --memory-leak-check
// RUN: -DSYMEX_HARNESS_PERTURB --overflow-check --unsigned-overflow-check --memory-leak-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$

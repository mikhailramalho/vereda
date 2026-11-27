#include "../symex_ssa_00/symex_ssa_00.c"
// Imported from ESBMC regression/esbmc/symex_ssa_00_fail
// Needs: --memory-leak-check
// RUN: -DSYMEX_HARNESS_PERTURB --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --unsigned-overflow-check --memory-leak-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none

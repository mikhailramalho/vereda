// extern_main.c
#include "extern.h"
int main() {
    __VERIFIER_assert(ASD[10] == 0); // Ok
    return 0;
}
// Imported from ESBMC regression/esbmc/github_481_2_false
// RUN: --no-incremental-bmc
// FILES: a.c
// EXPECT: ^VERIFICATION FAILED$

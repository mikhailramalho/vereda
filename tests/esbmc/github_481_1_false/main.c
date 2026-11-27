// extern_main.c
#include "extern.h"
int main() {
    __VERIFIER_assert(ASD[10] == 0); // Ok
    return 0;
}
// Imported from ESBMC regression/esbmc/github_481_1_false
// FILES: b.c
// EXPECT: ^VERIFICATION FAILED$

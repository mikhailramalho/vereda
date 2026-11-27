#include <assert.h>

int main() {
    int i = 0;
    int j = 0;
    int sum = 0;
    
    // Simple nested loops with invariants
    __ESBMC_loop_invariant(i >= 0 && i <= 2);
    while (i < 2) {
        j = 0;
        
        __ESBMC_loop_invariant(j >= 0 && j <= 3);
        while (j < 3) {
            sum++;
            j++;
        }
        i++;
    }
    
    assert(sum == 6); // 2 * 3 = 6
    return 0;
}
// Imported from ESBMC regression/loop-invariants/nested-2-simple_nested
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^WARNING: every violated claim lies downstream of a loop invariant havoc
// EXPECT: ^VERIFICATION UNKNOWN$

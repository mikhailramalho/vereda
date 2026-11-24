#include <assert.h>

int main() {
    int i = 0;
    int j = 0;
    int k = 0;
    int sum = 0;
    
    // Mixed case: outer and innermost loops have invariants, middle loop doesn't
    __ESBMC_loop_invariant(i >= 0 && i <= 2);
    while (i < 2) {
        j = 0;
        
        // Middle loop has no invariant
        while (j < 3) {
            k = 0;
            
            __ESBMC_loop_invariant(k >= 0 && k <= 4);
            while (k < 4) {
                sum++;
                k++;
            }
            j++;
        }
        i++;
    }
    
    assert(sum == 24); // 2 * 3 * 4 = 24
    return 0;
}
// Imported from ESBMC regression/loop-invariants/nested-5-mixed_invariants
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check
// MODE: FUTURE
// EXPECT: ^WARNING: every violated claim lies downstream of a loop invariant havoc
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none

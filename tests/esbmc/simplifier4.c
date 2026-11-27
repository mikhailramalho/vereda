#include <assert.h>

int main() {
    int x = __VERIFIER_nondet_int();
    unsigned u = __VERIFIER_nondet_uint();
    
    int zero_int = 0;
    assert((x ^ zero_int) == x);
    
    int zero_for_comparison = 0;
    assert((u < zero_for_comparison) == 0);
    
    assert((x ^ 0) == x);
    assert((u < 0) == 0);
    
    return 0;
}
// Imported from ESBMC regression/esbmc/simplifier4
// EXPECT: ^VERIFICATION SUCCESSFUL$

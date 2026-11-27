#include <assert.h>
#include <stdint.h>

extern uint16_t __VERIFIER_nondet_uint16_t();

int main() {
    uint16_t a = __VERIFIER_nondet_uint16_t();
    uint16_t b = __VERIFIER_nondet_uint16_t();
    uint16_t c = __VERIFIER_nondet_uint16_t(); // for nested checks

    // identity: (a & ~b) | (a ^ b) == (a ^ b)
    assert( (((a & ~b) | (a ^ b)) ) == ( (a ^ b) ) );

    // identity: (~a ^ b) | (a & b) == (~a ^ b)
    assert( ((((~a) ^ b) | (a & b)) ) == ( ((~a) ^ b) ) );

    // identity: (~a & b) | ~(a | b) == ~a
    assert( ((((~a) & b) | (~(a | b))) ) == ( (~a) ) );

    // identity: (a ^ b) | (a | b) == (a | b)
    assert( (((a ^ b) | (a | b)) ) == ( (a | b) ) );

    // Nested checks (replace LHS by RHS and combine with c)
    assert( ( (((a & ~b) | (a ^ b)) | c) ) == ( ((a ^ b) | c) ) );
    assert( ( (((a & ~b) | (a ^ b)) & c) ) == ( ((a ^ b) & c) ) );

    return 0;
}


// Imported from ESBMC regression/esbmc/simplifier9
// EXPECT: ^VERIFICATION SUCCESSFUL$

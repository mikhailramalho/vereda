#include <assert.h>
#include <stdint.h>

extern uint8_t __VERIFIER_nondet_uint8_t();

int main() {
    uint8_t a = __VERIFIER_nondet_uint8_t();
    uint8_t b = __VERIFIER_nondet_uint8_t();
    uint8_t c = __VERIFIER_nondet_uint8_t(); // for nested checks

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


// Imported from ESBMC regression/esbmc/simplifier8-no
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$

/*
 * Test for union dereference with dynamic/symbolic offset.
 *
 * Before the fix in dereference.cpp and symex_assign.cpp, ESBMC could not
 * handle direct union assignment through pointers at symbolic offsets.
 * construct_struct_ref_from_dyn_offs_rec() only handled struct types,
 * not unions.
 */

#include <stdint.h>
#include <assert.h>

typedef union {
    int16_t s;
    uint16_t r;
} slot_t;

slot_t stack[8];

int16_t __VERIFIER_nondet_s2(void);
uint8_t __VERIFIER_nondet_u1(void);

int main(void) {
    uint8_t sp = __VERIFIER_nondet_u1();
    __VERIFIER_assume(sp >= 1 && sp < 8);

    int16_t val = __VERIFIER_nondet_s2();
    stack[sp - 1].s = val;

    /* Direct union copy at symbolic offset - failed before fix */
    stack[sp] = stack[sp - 1];

    assert(stack[sp].s == val);
    assert(stack[sp].s == stack[sp - 1].s);

    return 0;
}
// Imported from ESBMC regression/esbmc/union-dynamic-offset-deref
// RUN: --ub-pointer-check=false --ub-div-by-zero-check=false
// EXPECT: ^VERIFICATION SUCCESSFUL$

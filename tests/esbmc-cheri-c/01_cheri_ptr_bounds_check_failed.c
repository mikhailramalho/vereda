#include <stdio.h>
#include <assert.h>
#include <cheri/cheric.h>

int main() {
    // A regular integer array on the stack
    int buffer[4] = {10, 20, 30, 40};

    // Create a CHERI capability pointer to 'buffer',
    // with bounds set to the size of the entire array (4 * sizeof(int))
    int *__capability cap_ptr = cheri_ptr(buffer, sizeof(buffer));

    // out of bounds access
    printf("Within bounds: %d\n", cap_ptr[4]);

    return 0;
}
// Imported from ESBMC regression/cheri-c/01_cheri_ptr_bounds_check_failed
// Needs: --cheri hybrid
// RUN: --cheri hybrid
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \bdereference failure: CHERI capability bounds violated\b
// Vereda: verdict none

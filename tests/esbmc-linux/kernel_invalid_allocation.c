#include <ubuntu20.04/kernel_5.15.0-76/include/linux/slab.h>

int main(void)
{
    void * ptr =  kmalloc(-2,2);

    return 0;
}
// Imported from ESBMC regression/linux/kernel_invalid_allocation
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none

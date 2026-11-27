#include <ubuntu20.04/kernel_5.15.0-76/include/linux/slab.h>
int main(void)
{
    void * ptr =  kmalloc(24,1);

    return 0;
}
// Imported from ESBMC regression/linux/kmalloc_valid
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none

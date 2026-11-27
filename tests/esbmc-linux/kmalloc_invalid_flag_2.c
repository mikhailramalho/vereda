#include <ubuntu20.04/kernel_5.15.0-76/include/linux/slab.h>
int main(void)
{
    void * ptr =  kmalloc(24,81);

    return 0;
}
// Imported from ESBMC regression/linux/kmalloc_invalid_flag_2
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none

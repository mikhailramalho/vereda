#include <ubuntu20.04/kernel_5.15.0-76/include/asm/uaccess.h>

int main()
{
    // Non-deterministic choice of indices
    int kernel_index = __VERIFIER_nondet_int();
    int user_index = __VERIFIER_nondet_int();

    // Constrain the indices to be within valid ranges
    __VERIFIER_assume(kernel_index >= 0 && kernel_index < KERNEL_MEMORY_SPACE);
    __VERIFIER_assume(user_index >= 0 && user_index < USER_MEMORY_SPACE);

    char* kernel_addr = kernel_memory + kernel_index;
    char* user_addr = user_memory + user_index;

    unsigned long size = __VERIFIER_nondet_int();

    __VERIFIER_assume(size < 200);
    __VERIFIER_assume(kernel_index + size <= KERNEL_MEMORY_SPACE);
    __VERIFIER_assume(user_index + size <= USER_MEMORY_SPACE);

    char kernel_buffer[size];
    memcpy(kernel_addr, kernel_buffer, size); 

    copy_to_user(user_addr, kernel_addr, size);

 
     return 0;


}
// Imported from ESBMC regression/linux/kernel_to_user_copy_valid
// RUN: --unwind 200 --std gnu89
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none

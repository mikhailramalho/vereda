#include <stdlib.h>
#include <stddef.h>

struct small_struct {
    char data[4];
};

int main() {
    struct small_struct *ptr = malloc(sizeof *ptr);
    
    // Try to access way beyond the allocated memory
    char *base = (char*)ptr;
    char *far_ptr = base + 1000;  // Way out of bounds
    
    // This should fail - accessing unallocated memory
    char bad_access = *far_ptr;
    
    free(ptr);
    return 0;
}
// Imported from ESBMC regression/esbmc/github_2512_6
// RUN: --force-malloc-success
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL

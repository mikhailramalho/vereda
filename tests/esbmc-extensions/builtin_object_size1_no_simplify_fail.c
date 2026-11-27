#include <assert.h>

int main() {
    char buffer[100];
    int numbers[10];
    
    assert(__builtin_object_size(buffer, 0) == 100);
    assert(__builtin_object_size(numbers, 0) == 41);
    
    return 0;
}
// Imported from ESBMC regression/extensions/builtin_object_size1_no_simplify_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$

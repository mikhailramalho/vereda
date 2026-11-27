#include <assert.h>

int main() {
    int x = 42;
    char c = 'a';
    double d = 3.14;
    
    assert(__builtin_object_size(&x, 0) == sizeof(int));
    assert(__builtin_object_size(&c, 0) == sizeof(char));
    assert(__builtin_object_size(&d, 0) == sizeof(double));
    
    return 0;
}

// Imported from ESBMC regression/extensions/builtin_object_size3
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$

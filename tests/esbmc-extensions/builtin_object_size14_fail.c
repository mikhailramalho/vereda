#include <assert.h>
#include <stdlib.h>

int main() {
    char *p;
    // Uninitialized pointer, object size cannot be determined
    size_t size = __builtin_object_size(p, 0);

    assert(size == 100);
    return 0;
}

// Imported from ESBMC regression/extensions/builtin_object_size14_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$

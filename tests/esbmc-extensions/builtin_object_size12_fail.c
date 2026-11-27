#include <assert.h>
#include <stdlib.h>

int main() {
    char buffer[50];
    char *ptr = buffer + 10;

    // Type=1 → remaining size after offset
    size_t size = __builtin_object_size(ptr, 1);

    assert(size == 50);
    return 0;
}

// Imported from ESBMC regression/extensions/builtin_object_size12_fail
// EXPECT: ^VERIFICATION FAILED$

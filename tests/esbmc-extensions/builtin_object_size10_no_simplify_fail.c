#include <assert.h>
#include <stdlib.h>

int main() {
    char small_buffer[5];
    size_t size = __builtin_object_size(small_buffer, 0);
    assert(size >= 100);  // This should fail since buffer is only 5 bytes
    return 0;
}
// Imported from ESBMC regression/extensions/builtin_object_size10_no_simplify_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$

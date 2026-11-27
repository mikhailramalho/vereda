#include <assert.h>

int main() {
    int matrix[5][10];
    assert(__builtin_object_size(matrix, 0) == 5 * 11 * sizeof(int));
    return 0;
}

// Imported from ESBMC regression/extensions/builtin_object_size4_fail
// EXPECT: ^VERIFICATION FAILED$
